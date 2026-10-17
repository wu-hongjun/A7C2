"""Restore locked source snapshots from archives, downloading missing archives.

Does not execute upstream code, fetch firmware, resolve moving branches, or
overwrite an existing snapshot. Run verify_sources.py afterward.
"""
import hashlib
import io
import json
from pathlib import Path
import tempfile
import tarfile
import urllib.request

ROOT = Path(__file__).resolve().parents[1]


def fetch():
    lock = json.loads((ROOT / "research/sources.lock.json").read_text())
    for source in lock["sources"]:
        archive = ROOT / source["archive"]
        if archive.exists():
            data = archive.read_bytes()
        else:
            request = urllib.request.Request(source["archive_url"],
                                             headers={"User-Agent": "A7C2-research"})
            with urllib.request.urlopen(request, timeout=120) as response:
                data = response.read()
        if hashlib.sha256(data).hexdigest() != source["archive_sha256"]:
            raise ValueError(f"Archive hash mismatch: {source['name']}")
        if not archive.exists():
            archive.parent.mkdir(parents=True, exist_ok=True)
            archive.write_bytes(data)
        destination = ROOT / source["source_directory"]
        if destination.exists():
            print(f"Kept existing snapshot: {source['name']}")
            continue
        destination.parent.mkdir(parents=True, exist_ok=True)
        with tempfile.TemporaryDirectory(dir=destination.parent) as temporary:
            staging = Path(temporary) / "snapshot"
            staging.mkdir()
            with tarfile.open(fileobj=io.BytesIO(data), mode="r:gz") as archive_file:
                members = archive_file.getmembers()
                prefix = members[0].name.split("/")[0] + "/"
                seen = set()
                expected = {row["path"]: row for row in source["files"]}
                for member in members:
                    if member.isdir():
                        continue
                    relative = member.name.removeprefix(prefix)
                    path = Path(relative)
                    if (not member.isfile() or not member.name.startswith(prefix)
                            or path.is_absolute() or ".." in path.parts
                            or relative not in expected or relative in seen):
                        raise ValueError(f"Unexpected archive member: {member.name}")
                    payload = archive_file.extractfile(member).read()
                    row = expected[relative]
                    if (len(payload) != row["bytes"]
                            or hashlib.sha256(payload).hexdigest() != row["sha256"]):
                        raise ValueError(f"Snapshot hash mismatch: {relative}")
                    target = staging / path
                    target.parent.mkdir(parents=True, exist_ok=True)
                    target.write_bytes(payload)
                    if "mode" in row:
                        target.chmod(int(row["mode"], 8))
                    seen.add(relative)
                if seen != set(expected):
                    raise ValueError(f"Incomplete archive: {source['name']}")
            staging.rename(destination)
        print(f"Restored {source['name']} @ {source['commit']}")


if __name__ == "__main__":
    fetch()
