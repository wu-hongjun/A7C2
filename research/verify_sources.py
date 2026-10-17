"""Verify downloaded archives and every snapshot file; never imports upstream code."""
import hashlib
import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def verify():
    lock = json.loads((ROOT / "research/sources.lock.json").read_text())
    count = 0
    for source in lock["sources"]:
        archive = ROOT / source["archive"]
        if digest(archive) != source["archive_sha256"]:
            raise ValueError(f"Archive mismatch: {archive}")
        directory = ROOT / source["source_directory"]
        expected = {row["path"] for row in source["files"]}
        actual = {p.relative_to(directory).as_posix()
                  for p in directory.rglob("*") if p.is_file()
                  and "__pycache__" not in p.parts}
        if actual != expected:
            raise ValueError(f"Snapshot file set differs: {source['name']}")
        for row in source["files"]:
            path = directory / row["path"]
            if path.stat().st_size != row["bytes"] or digest(path) != row["sha256"]:
                raise ValueError(f"Snapshot mismatch: {path}")
        count += len(expected)
        print(f"OK {source['name']} @ {source['commit']}: {len(expected)} files")
    print(f"Verified {len(lock['sources'])} archives and {count} snapshot files")
    for row in lock.get("supporting_files", []):
        path = ROOT / row["path"]
        if path.stat().st_size != row["bytes"] or digest(path) != row["sha256"]:
            raise ValueError(f"Supporting metadata mismatch: {path}")
    print(f"Verified {len(lock.get('supporting_files', []))} supporting metadata files")


if __name__ == "__main__":
    verify()
