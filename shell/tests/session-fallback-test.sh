#!/bin/sh
# Session de secours (ADR 0049) : réglages illisibles, puis deux arrêts immédiats → secours.
set -eu
SRC=$1
T=$(mktemp -d); trap 'rm -rf "$T"' EXIT
export XDG_STATE_HOME="$T/state" VINX_SAFE_NOTICE=true
cat > "$T/startplasma" <<'S'
#!/bin/sh
echo "${VINX_SAFE_MODE:-normal} ${VINX_SESSION:-0}" >> "$RUNS"
exit "${FAKE_STATUS:-0}"
S
chmod +x "$T/startplasma"
export VINX_STARTPLASMA="$T/startplasma" RUNS="$T/runs"
# Réglages valides sans effet (pas de kwriteconfig6 ni de systemctl ici).
printf 'vinx_setup_start() { export VINX_SESSION=1; }\nvinx_setup_stop() { :; }\n' > "$T/ok"
printf 'vinx_setup_start() {\n<<<<<<< HEAD\n}\n' > "$T/broken"
fail() { echo "ÉCHEC : $*"; cat "$RUNS"; exit 1; }

VINX_SESSION_SETUP="$T/broken" sh "$SRC/session/vinx-session" || true
[ "$(tail -1 "$RUNS")" = "setup 0" ] || fail "réglages illisibles → secours attendu"

VINX_SESSION_SETUP="$T/ok" sh "$SRC/session/vinx-session"
[ "$(tail -1 "$RUNS")" = "normal 1" ] || fail "session normale attendue"

for _ in 1 2; do VINX_SESSION_SETUP="$T/ok" FAKE_STATUS=1 sh "$SRC/session/vinx-session" || true; done
VINX_SESSION_SETUP="$T/ok" sh "$SRC/session/vinx-session"
[ "$(tail -1 "$RUNS")" = "crash 0" ] || fail "deux arrêts immédiats → secours attendu"

VINX_SESSION_SETUP="$T/ok" sh "$SRC/session/vinx-session"
[ "$(tail -1 "$RUNS")" = "normal 1" ] || fail "retour à la normale après le secours"
echo "session de secours : OK"
