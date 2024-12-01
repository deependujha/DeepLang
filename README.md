# DeepLang

## Setup

```bash
mkdir build && cd build
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ..
cmake --build .

./src/deeplang
```

---

## Using pre-commit hook to format and tidy

- `lint-code.sh` file contains the code to format and tidy the code.
- `.git/hooks/pre-commit` file contains the code to run the script.

```bash
# .git/hooks/pre-commit

#!/bin/bash

# run lint-code.sh
sh lint-code.sh

echo "Pre-commit checks complete."
```
