"""Download the two approved official research inputs; never access a camera.

Sony's EULA was explicitly accepted by the workspace owner on 2026-10-17.
URLs were read from the official support download controls after acceptance.
Firmware and extracted contents stay local, outside version control.
"""
from concurrent.futures import ThreadPoolExecutor
import hashlib
import json
from pathlib import Path
import urllib.request

ROOT = Path(__file__).resolve().parents[1]
INPUTS = [
    ("ILCE-7CM2", "2.01", 1139929560,
     "https://support.d-imaging.sony.co.jp/download/NEX/uua3b90658/BODYDATA.DAT?fm=am",
     "https://www.sony.com/electronics/support/e-mount-body-ilce-7-series/ilce-7cm2/software/00301105"),
    ("ILCE-1M2", "4.00", 1194332648,
     "https://support.d-imaging.sony.co.jp/download/NEX/lvb4f975ad/BODYDATA.DAT?fm=am",
     "https://www.sony.com/electronics/support/e-mount-body-ilce-1-series/ilce-1m2/software/00349486"),
]


def file_hash(path):
    sha = hashlib.sha256()
    with path.open("rb") as file:
        while block := file.read(4 * 1024 * 1024):
            sha.update(block)
    return sha.hexdigest()


def download(row):
    model, version, size, url, page = row
    directory = ROOT / "research/firmware" / f"{model}-{version}"
    directory.mkdir(parents=True, exist_ok=True)
    target = directory / "BODYDATA.DAT"
    if not target.exists():
        partial = target.with_suffix(".DAT.partial")
        if partial.exists():
            raise ValueError(f"Partial download exists; inspect before retrying: {partial}")
        request = urllib.request.Request(url, headers={"User-Agent": "A7C2-research"})
        count = 0
        with urllib.request.urlopen(request, timeout=60) as response, partial.open("xb") as file:
            declared = response.headers.get("Content-Length")
            if declared and int(declared) != size:
                raise ValueError(f"Unexpected official download length: {declared}")
            while block := response.read(4 * 1024 * 1024):
                count += file.write(block)
                if count % (128 * 1024 * 1024) == 0:
                    print(f"{model}: {count}/{size} bytes", flush=True)
            file.flush()
            __import__("os").fsync(file.fileno())
        if count != size:
            raise ValueError(f"Incomplete firmware download: {model}: {count}/{size}")
        partial.rename(target)
    if target.stat().st_size != size:
        raise ValueError(f"Existing download size mismatch: {target}")
    digest = file_hash(target)
    print(f"{model} {version}: downloaded and hashed {size} bytes", flush=True)
    return {"model": model, "version": version, "path": target.relative_to(ROOT).as_posix(),
            "bytes": size, "sha256": digest, "download_url": url,
            "support_page": page, "sha256_source": "locally measured; not a Sony-published digest"}


if __name__ == "__main__":
    with ThreadPoolExecutor(max_workers=2) as pool:
        results = list(pool.map(download, INPUTS))
    destination = ROOT / "research/firmware-inputs.json"
    if destination.exists():
        old = json.loads(destination.read_text())
        if [r["sha256"] for r in old["inputs"]] != [r["sha256"] for r in results]:
            raise ValueError("Downloaded firmware differs from the previously recorded inputs")
    destination.write_text(json.dumps({"date": "2026-10-17", "eula_accepted_by_user": True,
                                       "inputs": results}, indent=2) + "\n")
