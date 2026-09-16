#!/usr/bin/env bash
set -euo pipefail

REPO_NAME="SGDCORE"
REMOTE_URL="${1:-}"

echo "Initializing SGDCORE v0.1 repository..."

git init
git add .
git commit -m "SGDCORE v0.1: tape-based reverse-mode AD core"
git branch -M main

if [ -n "$REMOTE_URL" ]; then
    git remote add origin "$REMOTE_URL"
    git push -u origin main
    git tag -a v0.1.0 -m "SGDCORE v0.1.0"
    git push origin v0.1.0
    echo "Pushed to $REMOTE_URL and tagged v0.1.0"
else
    echo "No remote URL provided. Repository initialized locally."
    echo "To upload: ./setup_repo.sh https://github.com/sgPhilia/SGDCORE.git"
fi