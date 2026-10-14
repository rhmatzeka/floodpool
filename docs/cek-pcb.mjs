// Cek susunan PCB di panduan-pcb.html: tidak ada lubang dipakai dua kali, tidak ada korsleting antar-net,
// dan setiap net tersambung utuh. Jalankan: node docs/cek-pcb.mjs
import { readFileSync } from "node:fs";

const html = readFileSync(new URL("./panduan-pcb.html", import.meta.url), "utf8");
const D = JSON.parse(html.match(/<script type="application\/json" id="pcb-data">([\s\S]*?)<\/script>/)[1]);
const salah = [];
const key = (c, r) => `${c},${r}`;

// Kaki komponen: satu lubang satu kaki.
const netDi = new Map();
for (const p of D.parts) for (const [c, r, net] of p.holes) {
  if (c < 1 || c > D.cols || r < 1 || r > D.rows) salah.push(`${p.id}: K${c} B${r} di luar papan`);
  if (netDi.has(key(c, r))) salah.push(`K${c} B${r} dipakai dua kali (${p.id})`);
  netDi.set(key(c, r), net);
}

// Soket kiri harus sesuai urutan pin ESP32.
D.parts.find((p) => p.id === "hdrL").holes.forEach(([c, r, net], i) => {
  const pin = D.pins[i];
  if (c !== D.espCol || r !== D.espRow1 + i || net.replace(/^-/, "") !== pin) salah.push(`soket kiri baris ${r}: ${net} ≠ ${pin}`);
});

// Kawat/kaki menyambung semua lubang yang dilewatinya (anggap bersentuhan).
const induk = new Map();
const cari = (k) => { while (induk.has(k) && induk.get(k) !== k) k = induk.get(k); return k; };
const satukan = (a, b) => { induk.set(cari(a), cari(b)); };
for (const w of D.runs) {
  const [c1, r1] = w.from, [c2, r2] = w.to;
  if (c1 !== c2 && r1 !== r2) salah.push(`${w.id} miring`);
  const lewat = [];
  for (let c = Math.min(c1, c2); c <= Math.max(c1, c2); c++)
    for (let r = Math.min(r1, r2); r <= Math.max(r1, r2); r++) lewat.push(key(c, r));
  lewat.forEach((k) => { if (!induk.has(k)) induk.set(k, k); });
  lewat.slice(1).forEach((k) => satukan(lewat[0], k));
}
for (const k of netDi.keys()) if (!induk.has(k)) induk.set(k, k);

// Setiap kelompok tersambung hanya boleh berisi satu net; pin tak dipakai (awalan -) harus sendirian.
const kelompok = new Map();
for (const [k, net] of netDi) {
  const g = cari(k);
  if (!kelompok.has(g)) kelompok.set(g, new Set());
  kelompok.get(g).add(net);
}
for (const nets of kelompok.values()) {
  if (nets.size > 1) salah.push(`korsleting: ${[...nets].join(" + ")}`);
}
const kelompokNet = new Map();
for (const [k, net] of netDi) {
  if (net.startsWith("-")) continue;
  if (!kelompokNet.has(net)) kelompokNet.set(net, new Set());
  kelompokNet.get(net).add(cari(k));
}
for (const [net, gs] of kelompokNet) if (gs.size > 1) salah.push(`net ${net} terputus jadi ${gs.size} bagian`);
for (const [k, net] of netDi) if (net.startsWith("-") && [...netDi].some(([k2]) => k2 !== k && cari(k2) === cari(k))) salah.push(`pin tak dipakai ${net} ikut tersambung`);

// Rangkaian yang diharapkan (sama dengan breadboard yang sudah teruji).
const harus = {
  "3V3": ["12,12", "10,12", "9,12"],
  GND: ["12,25", "10,25", "9,25", "8,25", "14,25", "3,24", "5,17"],
  P32: ["12,18", "11,18"], LEDA: ["7,18", "5,18"],
  P33: ["12,19", "10,19"], P25: ["12,20", "10,20"],
  P26: ["12,21", "11,21", "14,21"], ECHO: ["7,21", "6,21"],
  P27: ["12,22", "3,22"], P14: ["12,23", "10,23"], P13: ["12,26", "10,26"], "5V": ["12,30", "10,30"],
};
for (const [net, lubang] of Object.entries(harus)) for (const k of lubang) {
  if (netDi.get(k) !== net) salah.push(`K${k.replace(",", " B")} harus ${net}, tertulis ${netDi.get(k)}`);
}

if (salah.length) { console.log("GAGAL\n- " + salah.join("\n- ")); process.exit(1); }
console.log(`OK: ${D.parts.length} komponen, ${D.runs.length} kawat, ${kelompokNet.size} net, tanpa korsleting`);
