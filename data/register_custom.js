import { Shelfs } from "/shelfs.js";
import { Parts } from "/parts.js";

const onRegister = function () {
    const part = {
        code: document.getElementById("code").value,
        name: document.getElementById("name").value,
        count: parseInt(document.getElementById("count").value),
        shelf_id: parseInt(document.getElementById("shelf_id").value),
        memo: document.getElementById("memo").value
    }
    console.log(part);
    Parts.post(part);
}

const shelfs = new Shelfs;
shelfs.update().then(value => {
    document.getElementById("input-shelf_id").appendChild(shelfs.getSelectElement("select-shelf_id", () => {}, null, "shelf_id", "shelf_id"));
});

window.onRegister = onRegister;
export {onRegister};