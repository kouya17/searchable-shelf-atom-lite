export class Parts {
    constructor() {
        this.value = [];
    }

    async update(cond) {
        let url = 'http://sshelf.local/api/parts';
        if (cond !== undefined && cond.name !== undefined) {
            url += '?name=' + cond.name;
        }
        const res = await fetch(url);
        this.value = await res.json();
        return this.value.parts;
    }

    static toQueryString(part) {
        return "?id=" + part.id + "&code=" + part.code + "&name=" + part.name
            + "&count=" + part.count + "&shelf_id=" + part.shelf_id
            + "&memo=" + part.memo + "&created_at=" + part.created_at
            + "&updated_at=" + part.updated_at;
    }

    static async post(part) {
        const res = await fetch('http://sshelf.local/api/parts', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
            },
            body: JSON.stringify(part),
        });
        return await res.json();
    }
}

export class PartClient {
    async update(id, part) {
        const res = await fetch('http://sshelf.local/api/parts/' + id, {
            method: 'PATCH',
            headers: {
                'Content-Type': 'application/json',
            },
            body: JSON.stringify(part)
        });
        return await res.json();
    }

    async delete(id) {
        const res = await fetch('http://sshelf.local/api/parts/' + id, {
            method: 'DELETE',
        });
        return await res.json();
    }
}