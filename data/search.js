import { Parts } from '/class/parts.js';
import { Search } from '/class/search.js';
import { Shelfs } from '/class/shelfs.js';

const parts = new Parts();

function updateParts(parts) {
    const divSearchResult = document.getElementById("search-result");
    while (divSearchResult.firstChild) {
        divSearchResult.removeChild(divSearchResult.firstChild);
    }
    parts.forEach(part => {
        var div = document.createElement("div");
        div.className = "search-result-div";
        var nameLink = document.createElement("a");
        nameLink.href = "/part.html" + Parts.toQueryString(part);
        var name = document.createElement("h2");
        name.innerHTML = part.name;
        nameLink.appendChild(name);
        div.appendChild(nameLink);
        var count = document.createElement("div");
        count.innerHTML = "個数: " + part.count;
        div.appendChild(count);
        var shelf = document.createElement("div");
        shelf.innerHTML = "引き出しid: " + part.shelf_id;
        div.appendChild(shelf);
        var memo = document.createElement("div");
        memo.innerHTML = "メモ: " + part.memo;
        div.appendChild(memo);
        var createdAt = document.createElement("div");
        createdAt.innerHTML = "作成日: " + part.created_at;
        div.appendChild(createdAt);
        var updatedAt = document.createElement("div");
        updatedAt.innerHTML = "更新日: " + part.updated_at;
        div.appendChild(updatedAt);
        divSearchResult.appendChild(div);
    });
}

const searchStr = (new URLSearchParams(document.location.search.substring(1))).get("name");
document.getElementById("input-search").defaultValue = searchStr;
parts.update({ name: searchStr }).then(parts => updateParts(parts));
Search.setSearchFunctions('input-search', 'button-search');
const shelfs = new Shelfs();
shelfs.update().then(v => { shelfs.allOff(); });