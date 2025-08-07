# 1. Shell starts a new shell process that executes the command string
sh -c "$(cat tmp/data/proc/shellcmdstr)"

# 2. Shell executes the file. Similar to `./tmp/data/proc/shellcmdstr`.
# if there is shebang in the script
sh "tmp/data/proc/shellcmdstr"

# 3. Read commands from stdin redirected from a file
sh < "tmp/data/proc/shellcmdstr"

# 4. Read commands from stdin from a pipe
cat "tmp/data/proc/shellcmdstr" | sh

# NOTE:
# 1. In #1, the command string has to be a one line string.
# 2. The first three ways all create one new process to execute.
# 3. The 4th way needs two processes: `cat` as pipe producer,
#    `sh` as pipe consumer. This way is inefficient compared with
#    the former three.
# 4. #2 is the suggested way for normal use.
