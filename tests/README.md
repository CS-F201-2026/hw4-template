# Your test cases

Add at least three cases to `student_cases.json`. It contains a JSON array:

```json
[
  {"name": "negative count", "input": "-1\n", "valid": false},
  {
    "name": "one zero reading",
    "input": "1\n12 0 C\n",
    "report": "Records: 1\nTotal temperature: 0\nFreezing: 0\nMinimum: 0\nMaximum: 0\nFreezing readings:\n"
  }
]
```

The outer array needs commas between cases, with no comma after the last case.
Write actual reasoning-driven cases: boundaries, empty data, and malformed files
are useful. A `report` is compared exactly. `valid: false` requires a nonzero exit
and no creation/truncation of the output file. Do not put credentials or private
student data in tests. The examples above illustrate the format; choose at least
three tests yourself. Run all cases using `make test`.

<!-- A4: AI-assisted instructor scaffolding. -->
