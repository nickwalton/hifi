//
//  miniTablet.js
//
//  Created by David Rowe on 20 Aug 2018.
//  Copyright 2018 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//

/* global EventBridge */
/* eslint-env browser */

(function () {

    "use strict";

    var // EventBridge
        READY_MESSAGE = "ready", // Engine <== Dialog
        HOVER_MESSAGE = "hover", // Engine <== Dialog
        MUTE_MESSAGE = "mute", // Engine <=> Dialog
        GOTO_MESSAGE = "goto", // Engine <=> Dialog
        EXPAND_MESSAGE = "expand", // Engine <== Dialog

        muteButton,
        muteImage,
        gotoButton,
        gotoImage,
        expandButton;

    // #region Communications ==================================================================================================

    function onScriptEventReceived(data) {
        var message;

        try {
            message = JSON.parse(data);
        } catch (e) {
            console.error("EventBridge message error");
            return;
        }

        switch (message.type) {
            case MUTE_MESSAGE:
                muteImage.src = message.icon;
                break;
            case GOTO_MESSAGE:
                gotoImage.src = message.icon;
                break;
        }
    }

    function onMuteButtonClick() {
        EventBridge.emitWebEvent(JSON.stringify({
            type: MUTE_MESSAGE
        }));
    }

    function onButtonHover() {
        EventBridge.emitWebEvent(JSON.stringify({
            type: HOVER_MESSAGE
        }));
    }

    function onGotoButtonClick() {
        EventBridge.emitWebEvent(JSON.stringify({
            type: GOTO_MESSAGE
        }));
    }

    function onExpandButtonClick() {
        EventBridge.emitWebEvent(JSON.stringify({
            type: EXPAND_MESSAGE
        }));
    }

    function connectEventBridge() {
        EventBridge.scriptEventReceived.connect(onScriptEventReceived);
        EventBridge.emitWebEvent(JSON.stringify({
            type: READY_MESSAGE
        }));
    }

    function disconnectEventBridge() {
        EventBridge.scriptEventReceived.disconnect(onScriptEventReceived);
    }

    // #endregion

    // #region Set-up and tear-down ============================================================================================

    function onUnload() {
        disconnectEventBridge();
    }

    function onLoad() {
        muteButton = document.getElementById("mute");
        muteImage = document.getElementById("mute-img");
        gotoButton = document.getElementById("goto");
        gotoImage = document.getElementById("goto-img");
        expandButton = document.getElementById("expand");

        connectEventBridge();

        muteButton.addEventListener("mouseenter", onButtonHover, false);
        gotoButton.addEventListener("mouseenter", onButtonHover, false);
        expandButton.addEventListener("mouseenter", onButtonHover, false);
        muteButton.addEventListener("click", onMuteButtonClick, true);
        gotoButton.addEventListener("click", onGotoButtonClick, true);
        expandButton.addEventListener("click", onExpandButtonClick, true);

        document.body.onunload = function () {
            onUnload();
        };
    }

    onLoad();

    // #endregion

}());
