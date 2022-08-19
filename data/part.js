import { Shelfs } from "/class/shelfs.js";
import { PartClient } from "/class/parts.js";

const partClient = new PartClient();
function onShelfChange(obj) {
    const idx = obj.target.selectedIndex;
    console.log(idx);
    const value = obj.target.options[idx].value;
    partClient.update((new URLSearchParams(window.location.search)).get("id"), {
        "shelf_id": value
    })
}

function renderPartDetail(shelfs, params) {
    const keys = [
        "name",
        "code",
        "count",
        // "shelf_id",
        "created_at",
        "updated_at"
    ];
    keys.forEach(key => {
        document.getElementById("part-" + key).textContent = params.get(key);
    })
    document.getElementById("part-shelf_id").appendChild(shelfs.getSelectElement("part-shelf_id-select", onShelfChange, params.get("shelf_id")));
}

var isLedOn = false;
function blinkLed(port) {
    if (isLedOn) {
        fetch('http://sshelf.local/api/ports/' + port + '/off');
        isLedOn = false;
    } else {
        fetch('http://sshelf.local/api/ports/' + port + '/on');
        isLedOn = true;
    }
}

var blinkTimer = null;
const shelfs = new Shelfs();
shelfs.update().then(value => {
    const params = new URLSearchParams(window.location.search);
    renderPartDetail(shelfs, params);
    blinkTimer = setInterval(blinkLed, 1000, shelfs.getPort(parseInt(params.get("shelf_id"))));
});

function onDeleteButtonClick() {
    partClient.delete((new URLSearchParams(window.location.search)).get("id")).then(res => {
        if ('error' in res) {
            document.getElementById("delete-log").textContent = res.error.message;
        } else {
            document.getElementById("delete-log").textContent = "削除しました";
        }
    });
}
document.getElementById('button-delete').addEventListener('click', onDeleteButtonClick);