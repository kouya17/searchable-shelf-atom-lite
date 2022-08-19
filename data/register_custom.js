import { Shelfs } from "/class/shelfs.js";
import { Parts } from "/class/parts.js";

function clearInputs() {
    document.getElementById("code").value = "";
    document.getElementById("name").value = "";
    document.getElementById("count").value = "";
    document.getElementById("shelf_id").value = 1;
    document.getElementById("memo").value = "";
}

const onRegister = function () {
    const part = {
        code: document.getElementById("code").value,
        name: document.getElementById("name").value,
        count: parseInt(document.getElementById("count").value),
        shelf_id: parseInt(document.getElementById("shelf_id").value),
        memo: document.getElementById("memo").value
    }
    console.log(part);
    Parts.post(part).then(res => {
        console.log("succeeded to register.")
        document.getElementById("result").innerText = part.name + "を登録しました！";
        clearInputs();
    });
}

const shelfs = new Shelfs;
shelfs.update().then(value => {
    document.getElementById("input-shelf_id").appendChild(shelfs.getSelectElement("select-shelf_id", () => {}, null, "shelf_id", "shelf_id"));
});

window.onRegister = onRegister;
export {onRegister};