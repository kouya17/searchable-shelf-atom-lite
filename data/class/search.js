export class Search {
    static setSearchFunctions(inputId, buttonId) {
        const jumpToSearchPage = function () {
            location.href = '/search.html?name=' + document.getElementById(inputId).value;
        };
        const onEnter = function (e) {
            if (e.keyCode === 13) {
                jumpToSearchPage();
            }
        };
        document.getElementById(inputId).oninput = function() {
            document.getElementById(buttonId).onclick = jumpToSearchPage;
            document.getElementById(inputId).addEventListener('keypress', onEnter);
        };
    }
}