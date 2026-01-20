import React from "react";
import { Card, Typography, Table } from "antd";

const { Paragraph } = Typography;

export default function TableResult({ data }) {
  if (!data) return null;
  if (data.error) {
    return (
      <Card title="错误">
        <Paragraph type="danger">{data.error}</Paragraph>
      </Card>
    );
  }

  // If backend provided structured columns/rows, render as a table
  const cols = data.columns;
  const rows = data.rows;
  if (cols && Array.isArray(cols) && rows && Array.isArray(rows)) {
    // sanitize header names (remove newlines, collapse whitespace)
    const cleanHeader = (h) => {
      if (h === null || h === undefined) return "";
      let s = typeof h === "string" ? h : (h && h.name) ? String(h.name) : String(h);
      s = s.replace(/\\r\\n/g, " ").replace(/\\n/g, " ").replace(/\\r/g, " ");
      s = s.replace(/\s+/g, " ").trim();
      return s;
    };

    const headerNames = cols.map(cleanHeader);

    // build table column config with fixed min width and ellipsis, enable horizontal scroll
    const tableColumns = headerNames.map((name, i) => ({
      title: name || `col${i}`,
      dataIndex: name,
      key: name || `col${i}`,
      ellipsis: true,
      width: 150,
    }));

    // Heuristics: prefer using keys from first row when available (they are usually the true column names)
    const useRowKeys = rows && rows.length > 0 && !Array.isArray(rows[0]) && Object.keys(rows[0]).length > 0;
    let finalHeaderNames = headerNames;

    // helper to detect bad headers (numeric / appear to be row values)
    const isBadHeaders = (hdrs, sampleRow) => {
      if (!hdrs || hdrs.length === 0) return true;
      let numericCount = 0;
      let shortCount = 0;
      hdrs.forEach(h => {
        if (/^-?\\d+(\\.\\d+)?$/.test(String(h).trim())) numericCount++;
        if ((String(h).trim() || "").length <= 2) shortCount++;
      });
      const numericRatio = numericCount / hdrs.length;
      const shortRatio = shortCount / hdrs.length;
      // if many headers are numeric or very short, treat as bad
      if (numericRatio > 0.4 || shortRatio > 0.5) return true;
      // if sampleRow contains many header-like values, treat as bad
      if (sampleRow) {
        const vals = Object.values(sampleRow).map(v => String(v).trim());
        const overlap = hdrs.filter(h => vals.includes(String(h).trim())).length;
        if (overlap / Math.max(1, hdrs.length) > 0.4) return true;
      }
      return false;
    };

    if (useRowKeys) {
      const firstRow = rows[0];
      const candidate = Object.keys(firstRow).filter(k => k !== "__rowid" && k !== "key");
      const cleanedCandidate = candidate.map(cleanHeader);
      if (isBadHeaders(headerNames, firstRow) && cleanedCandidate.length > 0) {
        finalHeaderNames = cleanedCandidate;
      }
    }

    // build mapping from cleaned header -> original key (prefer exact match)
    const cleanedToOriginal = {};
    if (rows && rows.length > 0 && !Array.isArray(rows[0])) {
      const first = rows[0];
      Object.keys(first).forEach((origKey) => {
        const ck = cleanHeader(origKey);
        if (!(ck in cleanedToOriginal)) cleanedToOriginal[ck] = origKey;
      });
    }

    // normalize rows into objects keyed by cleaned header names
    const dataSource = (rows || []).map((r, idx) => {
      const obj = {};
      if (Array.isArray(r)) {
        finalHeaderNames.forEach((h, i) => {
          obj[h] = r[i] !== undefined ? r[i] : "";
        });
      } else if (r && typeof r === "object") {
        Object.keys(r).forEach((origKey) => {
          const ck = cleanHeader(origKey);
          const targetKey = finalHeaderNames.includes(ck) ? ck : ck;
          obj[targetKey] = r[origKey];
        });
        // also ensure any finalHeaderNames missing are added as empty
        finalHeaderNames.forEach(h => { if (!(h in obj)) obj[h] = ""; });
      }
      obj.key = obj.key || obj.__rowid || idx;
      return obj;
    });

    const scrollX = Math.max(finalHeaderNames.length * 150, 800);
    const finalColumns = (finalHeaderNames || headerNames).map((name, i) => ({
      title: name || `col${i}`,
      dataIndex: name,
      key: name || `col${i}`,
      ellipsis: true,
      width: 150,
    }));

    return (
      <Card title="结果">
        <Table
          columns={finalColumns}
          dataSource={dataSource}
          pagination={false}
          size="small"
          scroll={{ x: scrollX }}
        />
      </Card>
    );
  }

  // If no structured data, attempt to parse stdout/raw_stdout into table (reuse TableView heuristics)
  const rawText = data.stdout || data.raw_stdout || data.stderr || "";
  const tryParse = (text) => {
    if (!text) return null;
    let t = text;
    try {
      // convert literal escaped newlines into actual newlines if present
      if (t.indexOf("\\n") !== -1 && t.indexOf("\n") === -1) {
        t = t.replace(/\\r\\n/g, "\n").replace(/\\n/g, "\n").replace(/\\r/g, "\n");
      }
    } catch (e) {}
    const lines = t
      .split("\n")
      .map((l) => l.replace(/\r/g, "").trim())
      .filter((l) => {
        if (!l) return false;
        const lower = l.toLowerCase();
        if (lower.startsWith("[exit]")) return false;
        if (/^\[.*\]/.test(l)) return false;
        return true;
      });

    if (!lines || lines.length === 0) return null;

    // If contains comma-separated header
    if (lines.length >= 2 && lines[0].indexOf(",") !== -1) {
      const headerLine = lines[0];
      const header = headerLine.split(",").map(h => h.trim()).filter(Boolean).map(h => {
        // if header has table prefix like customer.age, take suffix
        const parts = h.split(".");
        return parts[parts.length - 1];
      });
      const dataLines = lines.slice(1).filter(l => l.trim());
      const rowsParsed = dataLines.map((ln, idx) => {
        const cols = ln.split(",").map(c => c.trim());
        const obj = {};
        header.forEach((h, i) => {
          obj[h] = cols[i] !== undefined ? cols[i] : "";
        });
        obj.__rowid = idx;
        return obj;
      });
      return { columns: header, rows: rowsParsed };
    }

    // fallback: try pipe-separated or whitespace (not implemented here)
    return null;
  };

  const parsedFallback = tryParse(rawText);
  if (parsedFallback) {
    const { columns: pCols, rows: pRows } = parsedFallback;
    const tableColumns = pCols.map((name, i) => ({
      title: name,
      dataIndex: name,
      key: name || `col${i}`,
      ellipsis: true,
      width: 150,
    }));
    const dataSource = (pRows || []).map((r, idx) => ({ key: r.__rowid || idx, ...r }));
    return (
      <Card title="结果">
        <Table columns={tableColumns} dataSource={dataSource} pagination={false} size="small" scroll={{ x: Math.max(pCols.length * 150, 800) }} />
      </Card>
    );
  }

  // Final fallback: show raw text
  return (
    <Card title="结果">
      <pre style={{ whiteSpace: "pre-wrap" }}>{data.stdout || data.stderr}</pre>
    </Card>
  );
}


