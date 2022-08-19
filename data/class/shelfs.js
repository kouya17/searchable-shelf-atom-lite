export class Shelfs {
    value = [];

    async update() {
        let url = 'http://sshelf.local/api/shelfs';
        const res = await fetch(url);
        this.value = await res.json();
        return this.value;
    }

    getSelectElement(className, onChange, selectedId = null, name = null, id = null) {
        var select = document.createElement("select");
        select.className = className;
        select.onchange = onChange;
        if (name !== null) {
            select.name = name;
        }
        if (id !== null) {
            select.id = id;
        }
        return this.value.reduce((pre, shelf) => {
            let option = document.createElement("option");
            option.value = shelf.id;
            option.text = shelf.name;
            if (selectedId !== null && shelf.id == selectedId) {
                option.selected = true;
            }
            pre.appendChild(option);
            return pre;
        }, select);
    }

    getPort(shelf_id) {
        return this.value[shelf_id - 1].port;
    }

    allOff() {
        fetch('http://sshelf.local/api/ports/off');
    }
}