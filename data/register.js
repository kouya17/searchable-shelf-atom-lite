var shelfList = [
    {
        "id": -1,
        "name": "読み込み中",
        "port": -1,
        "memo": "読み込み中"
    }
];
const partsTbodyId = "table-parts";
const partRowId = "input-part-";
const codeCellClass = "part-code";
const nameCellClass = "part-name";
const countCellClass = "part-count";
const shelfCellClass = "part-shelf";
const memoCellClass = "part-memo";

fetch("http://searchable-shelf.local/api/shelfs")
    .then(res => res.json())
    .then(data => {
        shelfList = data;
    })

function convertPartsTextToPartInfoList(text) {
    const parts = text.split(/\r\n|\n/);
    const partInfoList = [];
    parts.forEach(part => {
        //const splited = part.split("　")
        //partInfoList.push([splited.shift().slice(1, -1), splited.join("　")]);
        const splited = part.split("\t");
        splited[1] = splited[1].slice(0, splited[1].length/2);
        splited[2] = splited[2].replace(/[^0-9]/g, '');
        partInfoList.push(splited);
    });
    return partInfoList;
}

function insertPartInfoListToTable(table, partInfoList) {
    partInfoList.forEach((partInfo, i) => {
        const row = table.insertRow(-1);
        row.id = partRowId + i;

        const cell_code = row.insertCell(-1);
        cell_code.className = codeCellClass;
        cell_code.innerHTML = partInfo[0];

        const cell_name = row.insertCell(-1);
        cell_name.className = nameCellClass;
        cell_name.innerHTML = partInfo[1];
        var input = document.createElement("input");
        input.className = countCellClass;
        input.type = "number";
        input.defaultValue = partInfo[2];
        row.insertCell(-1).appendChild(input);

        var select = document.createElement("select");
        select.className = shelfCellClass;
        select = shelfList.reduce((pre, shelf) =>{
            let option = document.createElement("option");
            option.value = shelf.id;
            option.text = shelf.name;
            pre.appendChild(option);
            return pre;
        }, select);
        row.insertCell(-1).appendChild(select);

        var memo = document.createElement("textarea");
        memo.className = memoCellClass;
        row.insertCell(-1).appendChild(memo);
    });
}

function replacePartTable(table, partInfoList) {
    while (table.firstChild) {
        table.removeChild(table.firstChild)
    }
    insertPartInfoListToTable(table, partInfoList)
}

function convertPartsList() {
    console.log("* convert parts list");
    const partsText = document.getElementById("input-parts").value;
    const partInfoList = convertPartsTextToPartInfoList(partsText);
    console.log(partInfoList);
    replacePartTable(document.getElementById(partsTbodyId), partInfoList);
}

async function postPart(part) {
    const res = await fetch('http://searchable-shelf.local/api/parts', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json',
        },
        body: JSON.stringify(part),
    });
    return await res.json();
}

async function postPartsTable() {
    console.log("* post parts table");
    const partsTbody = document.getElementById(partsTbodyId);
    const parts = [];
    for (var i = 0; i < partsTbody.rows.length; i++) {
        const part = {
            code: partsTbody.rows[i].querySelector('.' + codeCellClass).textContent,
            name: partsTbody.rows[i].querySelector('.' + nameCellClass).textContent,
            count: parseInt(partsTbody.rows[i].querySelector('.' + countCellClass).value),
            shelf_id: parseInt(partsTbody.rows[i].querySelector('.' + shelfCellClass).value),
            memo: partsTbody.rows[i].querySelector('.' + memoCellClass).textContent
        }
        parts.push(part);
    }
    console.log(parts);
    parts.forEach(async (part) => {
        await postPart(part);
    });
    document.getElementById("result").textContent = "登録しました！";
}