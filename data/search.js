import { Parts } from '/class/parts.js';
import { Search } from '/class/search.js';
import { Shelfs } from '/class/shelfs.js';

const parts = new Parts();

function generateFuncJumpToSearchPage(name, page) {
    return function () {
        location.href = '/search.html?name=' + name + "&page=" + page;
    };
}

function updateParts(res, searchStr, nowPage) {
    console.log(res);
    const divSearchResult = document.getElementById("search-result");
    while (divSearchResult.firstChild) {
        divSearchResult.removeChild(divSearchResult.firstChild);
    }
    res.parts.forEach(part => {
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
    const divPager = document.getElementById("pager");
    var backButton = document.createElement("button");
    backButton.innerHTML = "<<";
    backButton.onclick = generateFuncJumpToSearchPage(searchStr, nowPage - 1);
    if (nowPage < 2) backButton.setAttribute("disabled", true);
    divPager.appendChild(backButton);
    for (let i = 0; i < res.maxPage; i++) {
        var button = document.createElement("button");
        button.innerHTML = i + 1;
        button.onclick = generateFuncJumpToSearchPage(searchStr, i + 1);
        if (nowPage == i + 1) button.setAttribute("disabled", true);
        divPager.appendChild(button);
    }
    var forwardButton = document.createElement("button");
    forwardButton.innerHTML = ">>";
    forwardButton.onclick = generateFuncJumpToSearchPage(searchStr, nowPage + 1);
    if (nowPage > res.maxPage - 1) forwardButton.setAttribute("disabled", true);
    divPager.appendChild(forwardButton);
}

const searchStr = (new URLSearchParams(document.location.search.substring(1))).get("name");
let nowPage = (new URLSearchParams(document.location.search.substring(1))).get("page");
if (!nowPage) nowPage = 1;
document.getElementById("input-search").defaultValue = searchStr;
parts.update({ name: searchStr, page: nowPage }).then(res => updateParts(res, searchStr, parseInt(nowPage)));
Search.setSearchFunctions('input-search', 'button-search');
const shelfs = new Shelfs();
shelfs.update().then(v => { shelfs.allOff(); });