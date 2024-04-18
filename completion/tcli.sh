#/usr/bin/env bash

exe='tcli'

log2Comp() {
    local content=$1
    echo -e "$content" >>/tmp/comp
}

logArgs() {
    local args
    for ((i = 0; i < 10; i++)); do
        args+=("${i}:${COMP_WORDS[$i]}")
    done
    log2Comp "${args[*]}"
}

_tcli_completion() {
    # log2Comp "=== $(date +"%Y-%m-%d %H:%M:%S") ==="
    # logArgs
    # log2Comp "\ncur: ${COMP_WORDS[COMP_CWORD]}\nprev: ${COMP_WORDS[COMP_CWORD - 1]}\n2: $2\n3: $3"

    local temp_words="${COMP_WORDS[@]:0:$COMP_CWORD}"
    local output=$(${temp_words[@]} -l -s)
    COMPREPLY=($(compgen -W "$output" -- "$2"))
}

getExecs() {
    local prefix=$1
    local execs=()
    execs+=($(compgen -c ${prefix}))
    for ((i = 0; i < ${#execs[@]}; i++)); do
        execs[i]=$(basename ${execs[$i]})
    done
    execs+=(${prefix})
    printf "%s\n" "${execs[@]}" | sort | uniq
}

readarray -t execs <<< "$(getExecs ${exe})"

for tcli in ${execs[@]}; do
    complete -o default -F _tcli_completion ${tcli}
done
