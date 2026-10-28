#!/usr/bin/env python3
"""A4: AI-assisted instructor practice tests. No third-party packages needed."""
import json
from pathlib import Path
import subprocess
import sys
import tempfile


def report(rows):
    temps = [r[1] for r in rows]
    return (f"Records: {len(rows)}\nTotal temperature: {sum(temps)}\n"
            f"Freezing: {sum(t < 0 for t in temps)}\n"
            f"Minimum: {min(temps) if temps else 'N/A'}\n"
            f"Maximum: {max(temps) if temps else 'N/A'}\nFreezing readings:\n"
            + ''.join(f"{i} {t} C\n" for i, t in rows if t < 0))


def public_cases():
    rows = [(101, -5), (205, 0), (101, 8), (310, -2), (450, 4)]
    return [
        dict(name='example', input='5\n' + ''.join(f'{i} {t} C\n' for i, t in rows), report=report(rows)),
        dict(name='empty collection', input='0', report=report([])),
        dict(name='all negative, no final newline', input='2\n1 -100 C\n2 -1 C', report=report([(1,-100),(2,-1)])),
        dict(name='zero is not freezing', input='1\n1 0 C\n', report=report([(1,0)])),
        dict(name='partial record', input='1\n1 -5\n', valid=False),
        dict(name='extra record', input='0\n1 1 C\n', valid=False),
        dict(name='invalid unit', input='1\n1 5 F\n', valid=False),
        dict(name='missing file', missing=True, valid=False),
    ]


def check_case(exe, case):
    with tempfile.TemporaryDirectory(prefix='hw4-') as directory:
        root = Path(directory)
        src, dst = root / 'input data.txt', root / 'report.txt'
        if not case.get('missing'):
            src.write_text(case['input'])
        valid = case.get('valid', True)
        # A rejected input must leave an existing output file unchanged.
        if not valid:
            dst.write_text('KEEP EXISTING OUTPUT\n')
        run = subprocess.run([str(exe), str(src), str(dst)],
                             capture_output=True, text=True, timeout=10)
        assert 'AddressSanitizer' not in run.stderr and 'runtime error:' not in run.stderr, run.stderr
        if valid:
            assert run.returncode == 0, f'exit {run.returncode}: {run.stderr}'
            assert run.stdout == '' and run.stderr == '', 'unexpected console output'
            assert dst.is_file(), 'report not created'
            assert dst.read_text() == case['report'], f'wrong report: {dst.read_text()!r}'
        else:
            assert run.returncode != 0, 'invalid input accepted'
            assert run.stderr.strip(), 'missing diagnostic'
            assert dst.read_text() == 'KEEP EXISTING OUTPUT\n', 'output changed after failed load'
            dst.unlink()
            second = subprocess.run([str(exe), str(src), str(dst)], capture_output=True, text=True, timeout=10)
            assert second.returncode != 0 and not dst.exists(), 'failed load created output'
            assert 'AddressSanitizer' not in second.stderr and 'runtime error:' not in second.stderr, second.stderr


def main():
    exe = Path(sys.argv[1] if len(sys.argv) > 1 else './sensor_report').resolve()
    cases = public_cases()
    student = Path(__file__).with_name('student_cases.json')
    custom = json.loads(student.read_text())
    if not isinstance(custom, list):
        raise ValueError('student_cases.json must contain an array')
    for case in custom:
        if not isinstance(case, dict) or not isinstance(case.get('name'), str) or not isinstance(case.get('input'), str):
            raise ValueError('Each student case needs name and input strings')
        if case.get('valid', True) is not False and not isinstance(case.get('report'), str):
            raise ValueError('A success case needs an exact report string')
    cases.extend(custom)
    failed = 0
    for case in cases:
        try:
            check_case(exe, case)
            print('PASS:', case['name'])
        except (AssertionError, OSError, subprocess.TimeoutExpired) as error:
            failed += 1
            print('FAIL:', case['name'], '-', error)
    print(f'{len(cases)-failed}/{len(cases)} cases passed; {len(custom)} student cases (at least 3 per student required).')
    return bool(failed)


if __name__ == '__main__':
    sys.exit(main())
