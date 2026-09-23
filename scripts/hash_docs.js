#!/usr/bin/env node
// Rewrites docs/index.html's script cache-busting query params (?v=...) to a
// short content hash of each file, so the tag changes automatically whenever
// the file changes instead of relying on someone remembering to bump it.
const fs = require('fs');
const path = require('path');
const crypto = require('crypto');

const DOCS_DIR = path.join(__dirname, '..', 'docs');

function shortHash(filePath) {
    const data = fs.readFileSync(filePath);
    return crypto.createHash('sha256').update(data).digest('hex').slice(0, 8);
}

const networkHash = shortHash(path.join(DOCS_DIR, 'network.js'));
const mainHash = shortHash(path.join(DOCS_DIR, 'main.js'));

const indexPath = path.join(DOCS_DIR, 'index.html');
let html = fs.readFileSync(indexPath, 'utf8');
html = html.replace(/network\.js\?v=[^"]*/, `network.js?v=${networkHash}`);
html = html.replace(/main\.js\?v=[^"]*/, `main.js?v=${mainHash}`);
fs.writeFileSync(indexPath, html);

console.log(`network.js?v=${networkHash}`);
console.log(`main.js?v=${mainHash}`);
