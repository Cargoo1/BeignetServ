#!/usr/bin/env node
const fs = require('fs');

function sleep(ms) {
    return new Promise(resolve => setTimeout(resolve, ms));
}

function findFilename() {
    const query = process.env.QUERY_STRING || "";
    const target = "file=";
    const foundPos = query.indexOf(target);
    
    if (foundPos === -1) {
        return null;
    }
    
    const startIdx = foundPos + target.length;
    let sepPos = query.indexOf('&', foundPos);
    
    console.error(`${sepPos} ${foundPos}`); // Writes to stderr
    
    let filename = "";
    if (sepPos !== -1) {
        filename = query.substring(startIdx, sepPos);
    } else {
        filename = query.substring(startIdx);
    }
    
    console.error(filename);
    return filename;
}

function handleGet() {
    const filename = findFilename();
    if (!filename) return { status: 404, body: "" };

    try {
        fs.accessSync(filename, fs.constants.F_OK | fs.constants.R_OK);
        const body = fs.readFileSync(filename, 'utf-8');
        return { status: 0, body: body };
    } catch (e) {
        return { status: 404, body: "" };
    }
}

function handlePost(serverBody) {
    const filename = findFilename();
    if (!filename) return { status: 404, body: "" };

    const pathInfo = process.env.PATH_INFO;
    if (pathInfo !== undefined) {
        const filePath = "." + pathInfo;
        try {
            process.chdir(filePath);
        } catch (e) {
            return { status: -1, body: "" };
        }
    }

    console.error(filename);
    
    try {
        fs.writeFileSync(filename, serverBody);
    } catch (e) {
        console.error("Not open");
    }

    return { status: 0, body: "Hi from the script!!!" };
}

async function main() {
    await sleep(5000);

    // Read body from STDIN based on CONTENT_LENGTH
    let serverBody = "";
    const contentLength = parseInt(process.env.CONTENT_LENGTH || "0", 10);
    
    if (contentLength > 0) {
        const buffer = Buffer.alloc(contentLength);
        try {
            let bytesRead = 0;
            while (bytesRead < contentLength) {
                const r = fs.readSync(0, buffer, bytesRead, contentLength - bytesRead, null);
                if (r === 0) break;
                bytesRead += r;
            }
            serverBody = buffer.toString('utf-8', 0, bytesRead);
        } catch (e) {
            // Ignore if reading fails, identical to the C++ silent return
        }
    }

    const method = process.env.REQUEST_METHOD;
    let result = { status: -1, body: "" };

    if (method === "GET") {
        result = handleGet();
    } else if (method === "POST") {
        result = handlePost(serverBody);
    }

    if (result.status !== 0) {
        process.stdout.write("Status: 404 Not Found\r\n");
    }
    
    // Buffer.byteLength ensures correct counting for unicode characters
    const bodyLen = Buffer.byteLength(result.body);
    process.stdout.write(`Content-Length: ${bodyLen}\r\n\r\n`);
    process.stdout.write(result.body);
}

main();
