import { Shelfs } from '/class/shelfs.js';
import { Parts } from '/class/parts.js';
import { Search } from '/class/search.js';

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
    const res = await fetch('http://sshelf.local/api/shelfs');
    const resData = await res.json();
    console.log("get shelfs:", resData);
    return resData;
}

const addShelf = async function () {
    const name = document.getElementById("add-shelf-name").value;
    const port = parseInt(document.getElementById("add-shelf-port").value);
    const memo = document.getElementById("add-shelf-memo").value;
    console.log("add shelf: name=" + name + ", port=" + port + ", memo=" + memo);
    const data = {
        name: name,
        port: port,
        memo: memo
    }
    const res = await fetch('http://sshelf.local/api/shelfs', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json',
        },
        body: JSON.stringify(data),
    });
    const shelfs = await getShelfs();
    updateShelf(shelfs);
}
window.addShelf = addShelf;

const parts = new Parts();
async function getParts() {
    const res = await parts.update();
    console.log("get parts:", res);
    return res.parts;
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

Search.setSearchFunctions('input-search', 'button-search');
const shelfs = new Shelfs();
shelfs.update().then(value => {
    updateShelf(value);
    shelfs.allOff();
})
getParts().then(parts => updateParts(parts));