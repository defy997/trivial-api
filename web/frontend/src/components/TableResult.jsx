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
  // sanitize header names (remove newlines, collapse whitespace) - shared helper
  const cleanHeader = (h) => {
    if (h === null || h === undefined) return "";
    let s = typeof h === "string" ? h : (h && h.name) ? String(h.name) : String(h);
    s = s.replace(/\\r\\n/g, " ").replace(/\\n/g, " ").replace(/\\r/g, " ");
    s = s.replace(/\\s+/g, " ").trim();
    return s;
  };
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
      title: <span style={{ color: '#1890ff' }}>{name || `col${i}`}</span>,
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
      title: <span style={{ color: '#1890ff' }}>{name || `col${i}`}</span>,
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

    // Try to find one or more comma-separated table blocks.
    // A block is: headerLine (contains commas) followed by 1+ data lines (commas).
    const tables = [];
    let i = 0;
    while (i < lines.length) {
      const ln = lines[i];
      if (ln.indexOf(",") !== -1) {
        // treat as header
        const headerLine = ln;
        const header = headerLine
          .split(",")
          .map((h) => h.trim())
          .filter(Boolean)
          .map((h) => {
            const parts = h.split(".");
            return parts[parts.length - 1];
          });
        const rows = [];
        i++;
        while (i < lines.length && lines[i].indexOf(",") !== -1) {
          const cols = lines[i].split(",").map((c) => c.trim());
          const obj = {};
          header.forEach((h, idx2) => {
            obj[h] = cols[idx2] !== undefined ? cols[idx2] : "";
          });
          obj.__rowid = rows.length;
          rows.push(obj);
          i++;
        }
        tables.push({ columns: header, rows });
      } else {
        i++;
      }
    }
    if (tables.length === 1) {
      return { columns: tables[0].columns, rows: tables[0].rows };
    }
    if (tables.length > 1) {
      return { tables };
    }

    // fallback: try pipe-separated or whitespace (not implemented here)
    return null;
  };

  const parsedFallback = tryParse(rawText);
  if (parsedFallback) {
    // single table
    if (parsedFallback.columns && parsedFallback.rows) {
      const pCols = parsedFallback.columns;
      const pRows = parsedFallback.rows;
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
    // multiple tables: render stacked tables with a blank separator line and consistent header formatting
    if (parsedFallback.tables && Array.isArray(parsedFallback.tables)) {
      return (
        <Card title="结果">
          {parsedFallback.tables.map((tbl, idx) => {
            const origCols = tbl.columns || [];
            const pRows = tbl.rows || [];
            const cleanedCols = origCols.map(cleanHeader);
            // mapping original header -> cleaned header
            const origToClean = {};
            origCols.forEach((oc, i) => { origToClean[oc] = cleanedCols[i]; });

            const tableColumns = cleanedCols.map((name, i) => ({
              title: <span style={{ color: '#1890ff' }}>{name || `col${i}`}</span>,
              dataIndex: name,
              key: `${name || `col${i}`}-${idx}`,
              ellipsis: true,
              width: 150,
            }));

            // filter out rows that are just header names (e.g., parser produced header as a data row)
            const rawRows = pRows || [];
            const filteredRows = rawRows.filter((r) => {
              if (!r) return false;
              // if every column value equals the original header or cleaned header, treat as header-row and drop it
              let allMatchHeader = true;
              for (const oc of origCols) {
                const val = r[oc] !== undefined && r[oc] !== null ? String(r[oc]).trim() : "";
                const cleaned = String(origToClean[oc] || cleanHeader(oc)).trim();
                if (val === "") {
                  // empty cell -> not necessarily header-only, mark as not-matching header
                  allMatchHeader = false;
                  break;
                }
                if (val !== cleaned && val !== String(oc).trim()) {
                  allMatchHeader = false;
                  break;
                }
              }
              return !allMatchHeader;
            });

            const dataSource = (filteredRows || []).map((r, ridx) => {
              const obj = {};
              origCols.forEach((oc) => {
                const ck = origToClean[oc] || cleanHeader(oc);
                obj[ck] = r[oc] !== undefined ? r[oc] : "";
              });
              obj.key = `${idx}-${ridx}`;
              return obj;
            });

            return (
              <div key={`tbl-${idx}`} style={{ marginBottom: 20 }}>
                <div style={{ fontSize: 12, color: "#888", marginBottom: 8 }}>
                  {`Query ${idx + 1} - SELECT result`}
                </div>
                <Table columns={tableColumns} dataSource={dataSource} pagination={false} size="small" scroll={{ x: Math.max(cleanedCols.length * 150, 800) }} />
              </div>
            );
          })}
        </Card>
      );
    }
  }

  // Final fallback: show raw text
  return (
    <Card title="结果">
      <pre style={{ whiteSpace: "pre-wrap" }}>{data.stdout || data.stderr}</pre>
    </Card>
  );
}


