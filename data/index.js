function updateShelf(shelfs) {
    const ulShelfList = document.getElementById("shelf-list");
    while (ulShelfList.firstChild) {
        ulShelfList.removeChild(ulShelfList.firstChild);
    }
    shelfs.forEach(shelf => {
        var li = document.createElement("li");
        li.textContent = shelf.name + ", 接続ポート: " + shelf.port;
        ulShelfList.appendChild(li);
    });
}

async function getShelfs() {
    const res = await fetch('http://searchable-shelf.local/api/shelfs');
    const resData = await res.json();
    console.log("get shelfs:", resData);
    return resData;
}

async function addShelf() {
    const name = document.getElementById("add-shelf-name").value;
    const port = parseInt(document.getElementById("add-shelf-port").value);
    const memo = document.getElementById("add-shelf-memo").value;
    console.log("add shelf: name=" + name + ", port=" + port + ", memo=" + memo);
    const data = {
        name: name,
        port: port,
        memo: memo
    }
    const res = await fetch('http://searchable-shelf.local/api/shelfs', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json',
        },
        body: JSON.stringify(data),
    });
    const shelfs = await getShelfs();
    updateShelf(shelfs);
}

import { Parts } from '/parts.js';
const parts = new Parts();
async function getParts() {
    const partsValue = await parts.update();
    console.log("get parts:", partsValue);
    return partsValue
}

function updateParts(parts) {
    const ulPartList = document.getElementById("part-list");
    while (ulPartList.firstChild) {
        ulPartList.removeChild(ulPartList.firstChild);
    }
    parts.forEach(part => {
        var li = document.createElement("li");
        var link = document.createElement("a");
        link.href = "/part.html" + Parts.toQueryString(part);
        //link.innerHTML = part.name + ", 引き出しid: " + part.shelf_id;
        link.innerHTML = part.name;
        li.appendChild(link);
        ulPartList.appendChild(li);
    });
}

document.getElementById('input-search').oninput = function() {
    document.getElementById('button-search').onclick = function() {
        location.href = '/search.html?name=' + document.getElementById('input-search').value;
    };
};
getShelfs().then(shelfs => updateShelf(shelfs));
getParts().then(parts => updateParts(parts));