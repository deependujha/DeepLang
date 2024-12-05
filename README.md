# DeepLang

![work in progress](./assets/wip.jpg)

**DeepLang**: A lightweight, experimental interpreter framework designed to explore core concepts in language frontend, intermediate representation (IR), analysis and transformation passes, and specialized architecture backends. 🚀

---

## Setup

```bash
mkdir build && cd build
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ..
cmake --build .

./src/deeplang
```

---

<details>
  <summary>Using pre-commit hook to format and tidy</summary>

- `lint-code.sh` file contains the code to format and tidy the code.
- `.git/hooks/pre-commit` file contains the code to run the script.

```bash
# .git/hooks/pre-commit

#!/bin/bash

# run lint-code.sh
sh lint-code.sh

echo "Pre-commit checks complete."
```

</details>

if(4>3){4;}elif(5>7){3;}else{4*3;}
