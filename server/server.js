// Chat server: serves the web client and a plain-text API the calculator can
// parse without JSON.
//
//   GET  /messages?after=ID  ->  "id\tname\ttext\n" per message newer than ID
//   POST /messages           <-  "name\ntext"
//
// Node stdlib only. Run: node server/server.js  (PORT env, default 3000)

const http = require("node:http");
const fs = require("node:fs");
const path = require("node:path");

const PORT = process.env.PORT || 3000;
const KEEP = 100; // ponytail: in-memory, lost on restart; add a file/DB if history matters
const page = fs.readFileSync(path.join(__dirname, "index.html"));

const messages = [];
let nextId = 1;

// One line, no tabs, printable ASCII only (the calculator's font), capped.
const clean = (s, max) =>
  s.replace(/[^\x20-\x7e]/g, " ").trim().slice(0, max);

function add(body) {
  const nl = body.indexOf("\n");
  if (nl < 0) return false;
  const name = clean(body.slice(0, nl), 16);
  const text = clean(body.slice(nl + 1), 200);
  if (!name || !text) return false;
  messages.push({ id: nextId++, name, text });
  if (messages.length > KEEP) messages.shift();
  return true;
}

const list = (after) =>
  messages
    .filter((m) => m.id > after)
    .map((m) => `${m.id}\t${m.name}\t${m.text}\n`)
    .join("");

const server = http.createServer((req, res) => {
  const url = new URL(req.url, "http://x");

  if (url.pathname === "/" && req.method === "GET") {
    res.writeHead(200, { "Content-Type": "text/html; charset=utf-8" });
    return res.end(page);
  }

  if (url.pathname === "/messages" && req.method === "GET") {
    res.writeHead(200, { "Content-Type": "text/plain" });
    return res.end(list(Number(url.searchParams.get("after")) || 0));
  }

  if (url.pathname === "/messages" && req.method === "POST") {
    let body = "";
    req.setEncoding("utf8");
    req.on("data", (c) => {
      body += c;
      if (body.length > 1024) req.destroy();
    });
    req.on("end", () => {
      res.writeHead(add(body) ? 204 : 400);
      res.end();
    });
    return;
  }

  res.writeHead(404);
  res.end();
});

if (require.main === module) {
  server.listen(PORT, () => console.log(`chat on http://localhost:${PORT}`));
}

module.exports = { add, list, clean };
