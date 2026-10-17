/** Use REA's public local MCP launcher for offline native analysis only.
 * No agent registration, target execution, USB, or executable mutation.
 * Dependencies live under ignored research/build/tooling/rea-runtime.
 */
import { readFile, writeFile, mkdir, readdir } from 'node:fs/promises';
import { resolve, dirname, join } from 'node:path';
import { fileURLToPath, pathToFileURL } from 'node:url';

const here = dirname(fileURLToPath(import.meta.url));
const runtime = join(here, 'build/tooling/rea-runtime/node_modules');
const { Client } = await import(pathToFileURL(join(runtime, '@modelcontextprotocol/client/dist/index.mjs')));
const { StdioClientTransport } = await import(pathToFileURL(join(runtime, '@modelcontextprotocol/client/dist/stdio.mjs')));
const readonly = new Set(['binary_overview', 'procedure_pseudo_code', 'analyze_function',
  'procedure_instructions', 'procedure_callers', 'procedure_callees', 'xrefs',
  'list_segments', 'list_imports', 'list_exports', 'search_strings', 'search_names',
  'list_procedures', 'list_strings', 'read_bytes', 'address_to_file_offset',
  'binary_session', 'get_evidence_bundle', 'inspect_analysis_view', 'inspect_native_load_image']);
const specPath = process.argv[2];
if (!specPath) throw new Error('Usage: node research/run_rea.mjs /absolute/path/to/job.json');
const spec = JSON.parse(await readFile(resolve(specPath), 'utf8'));
if (spec.disabled_analyzers !== undefined &&
    (!Array.isArray(spec.disabled_analyzers) || spec.disabled_analyzers.some(
      name => typeof name !== 'string' || !name.trim() || name.includes(',')))) {
  throw new Error('disabled_analyzers must contain exact, nonempty analyzer names');
}
const output = resolve(spec.output_directory);
await mkdir(output, { recursive: true });
if ((await readdir(output)).length) throw new Error('Choose a new empty REA output directory: ' + output);
const client = new Client({ name: 'a7c2-offline-firmware-research', version: '1' });
const cancellation = new AbortController();
for (const signal of ['SIGINT', 'SIGTERM']) process.once(signal,
  () => cancellation.abort(new Error('Caller requested ' + signal)));
const transport = new StdioClientTransport({ command: process.execPath,
  args: [join(runtime, 'rea-agents/scripts/rea.mjs'), 'mcp'],
  env: { ...process.env,
    ...(spec.disabled_analyzers ? { REA_GHIDRA_DISABLED_ANALYZERS: spec.disabled_analyzers.join(',') } : {}),
    REA_ANALYSIS_PROVIDER: 'ghidra', REA_LOG_LEVEL: 'silent' }, stderr: 'pipe' });
let diagnostic = '';
transport.stderr?.on('data', b => { diagnostic = (diagnostic + b.toString()).slice(-131072); });
let opened = false;
const receipts = [];
async function call(name, args, file) {
  process.stderr.write(`REA ${name} ${args?.procedure ?? ''}\n`);
  const result = await client.callTool({ name, arguments: args ?? {} },
    { timeout: 1800000, ...(name === 'close_binary' ? {} : { signal: cancellation.signal }) });
  if (file) await writeFile(join(output, file), JSON.stringify(result, null, 2) + '\n', { flag: 'wx' });
  receipts.push({ operation: name, arguments: args, result_file: file,
    isError: result.isError === true, elapsed_at: new Date().toISOString() });
  if (result.isError) throw new Error(`${name} failed; see ${join(output, file)}`);
  return result;
}
try {
  await client.connect(transport);
  let cursor;
  const inventory = [];
  do {
    const page = await client.listTools(cursor ? { cursor } : {});
    inventory.push(...page.tools); cursor = page.nextCursor;
  } while (cursor);
  const schemas = inventory.filter(t => readonly.has(t.name) || ['open_binary', 'close_binary', 'export_evidence_bundle'].includes(t.name));
  await writeFile(join(output, 'tool-schemas.json'), JSON.stringify(schemas, null, 2) + '\n', { flag: 'wx' });
  if (spec.path) {
    await call('open_binary', { path: resolve(spec.path), provider_id: 'ghidra' }, 'open.json');
    opened = true;
    for (const [i, request] of (spec.queries ?? []).entries()) {
      if (!readonly.has(request.name)) throw new Error('Not an allowed static query: ' + request.name);
      await call(request.name, request.arguments, `${String(i).padStart(2, '0')}-${request.name}.json`);
    }
    if (spec.export_bundle) await call('export_evidence_bundle',
      { path: join(output, 'evidence-bundle.json') }, 'export.json');
  }
} finally {
  if (opened) {
    try { await call('close_binary', {}, 'close.json'); }
    catch (error) { receipts.push({ cleanup_error: String(error) }); process.exitCode = 1; }
  }
  await client.close();
  await transport.close();
  await writeFile(join(output, 'receipts.json'), JSON.stringify({ camera_accessed: false,
    target_execution_requested: false, operations: receipts, diagnostic }, null, 2) + '\n');
}
