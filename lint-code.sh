#!/bin/bash
# Pre-commit hook for formatting and tidy

# Format all staged C++ files
echo "Running clang-format..."
STAGED_FILES=$(git diff --cached --name-only --diff-filter=ACMR | grep -E '\.(cc|cpp|h)$')

for file in $STAGED_FILES; do
    clang-format -i  "$file" # uses .clang-format, but you can use `--style=Google` instead
    git add "$file"  # Re-add the modified files to the commit
done

# Run clang-tidy (optional: you can customize checks)
echo "Running clang-tidy..."
for file in $STAGED_FILES; do
    clang-tidy "$file" -quiet --fix -p build/compile_commands.json || true
    git add "$file"
done
