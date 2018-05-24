// ==UserScript==
// @name         Focus text input
// @description  Focus the first visible text input with 'Alt+i'
// @grant        none
// @match        *://*/*
// @require      //ajax.googleapis.com/ajax/libs/jquery/1.9.1/jquery.min.js
// ==/UserScript==

(function(window) {
    'use strict';

    window.addEventListener ("keydown", keyboardHandler, false);

    function keyboardHandler (zEvent) {
        var bBlockDefaultAction = false;

        if (zEvent.altKey && zEvent.which === window.KeyEvent.DOM_VK_I) {

            var firstInput = window.$("input:not(input[type=button],input[type=submit],select,button):visible:first");
            firstInput.focus();
            if (firstInput.scrollIntoView)
                firstInput.scrollIntoView();

            bBlockDefaultAction = true;
        }

        if (bBlockDefaultAction) {
            zEvent.preventDefault ();
            zEvent.stopPropagation ();
        }
    }
})(window);
