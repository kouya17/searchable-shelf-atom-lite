import { Shelfs } from "/shelfs.js";
import { PartClient } from "/parts.js";

const partClient = new PartClient();
function onShelfChange(obj) {
    const idx = obj.target.selectedIndex;
    console.log(idx);
    const value = obj.target.options[idx].value;
    partClient.update((new URLSearchParams(window.location.search)).get("id"), {
        "shelf_id": value
    })
}

function renderPartDetail(shelfs) {
    const params = new URLSearchParams(window.location.search);
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

const shelfs = new Shelfs();
shelfs.update().then(value => renderPartDetail(shelfs));