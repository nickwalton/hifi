//
//  Tree.qml
//
//  Created by David Rowe on 17 Feb 2016
//  Copyright 2016 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//

import QtQml.Models 2.2
import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

import "../styles-uit"

TreeView {
    id: treeView

    property var treeModel: ListModel { }
    property var canEdit: false
    property bool centerHeaderText: false
    property int colorScheme: hifi.colorSchemes.light
    readonly property bool isLightColorScheme: colorScheme == hifi.colorSchemes.light

    property var modifyEl: function(index, data) { return false; }

    model: treeModel
    selection: ItemSelectionModel {
        model: treeModel
    }

    anchors { left: parent.left; right: parent.right }
    
    headerVisible: false

    // Use rectangle to draw border with rounded corners.
    frameVisible: false
    Rectangle {
        color: "#00000000"
        anchors.fill: parent
        radius: hifi.dimensions.borderRadius
        border.color: isLightColorScheme ? hifi.colors.lightGrayText : hifi.colors.baseGrayHighlight
        border.width: 2
        anchors.margins: -2
    }
    anchors.margins: 2  // Shrink TreeView to lie within border.

    backgroundVisible: true

    horizontalScrollBarPolicy: Qt.ScrollBarAlwaysOff
    verticalScrollBarPolicy: Qt.ScrollBarAsNeeded

    style: TreeViewStyle {
        // Needed in order for rows to keep displaying rows after end of table entries.
        backgroundColor: parent.isLightColorScheme ? hifi.colors.tableRowLightEven : hifi.colors.tableRowDarkEven
        alternateBackgroundColor: parent.isLightColorScheme ? hifi.colors.tableRowLightOdd : hifi.colors.tableRowDarkOdd
        
        headerDelegate: Rectangle {
            height: hifi.dimensions.tableHeaderHeight
            color: isLightColorScheme ? hifi.colors.tableBackgroundLight : hifi.colors.tableBackgroundDark

            RalewayRegular {
                id: titleText
                text: styleData.value
                size: hifi.fontSizes.tableHeading
                font.capitalization: Font.AllUppercase
                color: hifi.colors.baseGrayHighlight
                horizontalAlignment: (centerHeaderText ? Text.AlignHCenter : Text.AlignLeft)
                elide: Text.ElideRight
                anchors {
                    left: parent.left
                    leftMargin: hifi.dimensions.tablePadding
                    right: sortIndicatorVisible && sortIndicatorColumn === styleData.column ? titleSort.left : parent.right
                    rightMargin: hifi.dimensions.tablePadding
                    verticalCenter: parent.verticalCenter
                }
            }

            HiFiGlyphs {
                id: titleSort
                text:  sortIndicatorOrder == Qt.AscendingOrder ? hifi.glyphs.caratUp : hifi.glyphs.caratDn
                color: isLightColorScheme ? hifi.colors.darkGray : hifi.colors.baseGrayHighlight
                opacity: 0.6;
                size: hifi.fontSizes.tableHeadingIcon
                anchors {
                    right: parent.right
                    verticalCenter: titleText.verticalCenter
                }
                visible: sortIndicatorVisible && sortIndicatorColumn === styleData.column
            }

            Rectangle {
                width: 1
                anchors {
                    left: parent.left
                    top: parent.top
                    topMargin: 1
                    bottom: parent.bottom
                    bottomMargin: 2
                }
                color: isLightColorScheme ? hifi.colors.lightGrayText : hifi.colors.baseGrayHighlight
                visible: styleData.column > 0
            }

            Rectangle {
                height: 1
                anchors {
                    left: parent.left
                    right: parent.right
                    bottom: parent.bottom
                }
                color: isLightColorScheme ? hifi.colors.lightGrayText : hifi.colors.baseGrayHighlight
            }
        }

        branchDelegate: HiFiGlyphs {
            text: styleData.isExpanded ? hifi.glyphs.caratDn : hifi.glyphs.caratR
            size: hifi.fontSizes.carat
            color: colorScheme == hifi.colorSchemes.light
                   ? (styleData.selected ? hifi.colors.black : hifi.colors.baseGrayHighlight)
                   : (styleData.selected ? hifi.colors.black : hifi.colors.lightGrayText)
            anchors {
                left: parent ? parent.left : undefined
                leftMargin: hifi.dimensions.tablePadding / 2
            }
        }

        handle: Item {
            id: scrollbarHandle
            implicitWidth: hifi.dimensions.scrollbarHandleWidth
            Rectangle {
                anchors {
                    fill: parent
                    topMargin: treeView.headerVisible ? hifi.dimensions.tableHeaderHeight + 3 : 3
                    bottomMargin: 3     // ""
                    leftMargin: 1       // Move it right
                    rightMargin: -1     // ""
                }
                radius: hifi.dimensions.scrollbarHandleWidth / 2
                color: treeView.isLightColorScheme ? hifi.colors.tableScrollHandleLight : hifi.colors.tableScrollHandleDark
            }
        }

        scrollBarBackground: Item {
            implicitWidth: hifi.dimensions.scrollbarBackgroundWidth
            Rectangle {
                anchors {
                    fill: parent
                    topMargin: treeView.headerVisible ? hifi.dimensions.tableHeaderHeight - 1 : -1
                    margins: -1     // Expand
                }
                color: treeView.isLightColorScheme ? hifi.colors.tableScrollBackgroundLight : hifi.colors.tableScrollBackgroundDark
                
                // Extend header color above scrollbar background
                Rectangle {
                    anchors {
                        top: parent.top
                        topMargin: -hifi.dimensions.tableHeaderHeight
                        left: parent.left
                        right: parent.right
                    }
                    height: hifi.dimensions.tableHeaderHeight
                    color: treeView.isLightColorScheme ? hifi.colors.tableBackgroundLight : hifi.colors.tableBackgroundDark
                    visible: treeView.headerVisible
                }
                Rectangle {
                    // Extend header bottom border
                    anchors {
                        top: parent.top
                        left: parent.left
                        right: parent.right
                    }
                    height: 1
                    color: treeView.isLightColorScheme ? hifi.colors.lightGrayText : hifi.colors.baseGrayHighlight
                    visible: treeView.headerVisible
                }
            }
        }

        incrementControl: Item {
            visible: false
        }

        decrementControl: Item {
            visible: false
        }
    }

    rowDelegate: Rectangle {
        height: hifi.dimensions.tableRowHeight
        color: styleData.selected
               ? hifi.colors.primaryHighlight
               : treeView.isLightColorScheme
                   ? (styleData.alternate ? hifi.colors.tableRowLightEven : hifi.colors.tableRowLightOdd)
                   : (styleData.alternate ? hifi.colors.tableRowDarkEven : hifi.colors.tableRowDarkOdd)
    }

    itemDelegate: Loader {
        id: itemDelegateLoader

        anchors {
            left: parent ? parent.left : undefined
            leftMargin: (2 + styleData.depth) * hifi.dimensions.tablePadding
            right: parent ? parent.right : undefined
            rightMargin: hifi.dimensions.tablePadding
            verticalCenter: parent ? parent.verticalCenter : undefined
        }

        function getComponent() {
            if (treeView.canEdit && styleData.selected) {
                return textFieldComponent;
            } else {
                return labelComponent;
            }

        }
        sourceComponent: getComponent()

        Component {
            id: labelComponent
            FiraSansSemiBold {

                text: styleData.value
                size: hifi.fontSizes.tableText
                color: colorScheme == hifi.colorSchemes.light
                       ? (styleData.selected ? hifi.colors.black : hifi.colors.baseGrayHighlight)
                       : (styleData.selected ? hifi.colors.black : hifi.colors.lightGrayText)
                       
                elide: Text.ElideRight
            }
        }
        Component {
            id: textFieldComponent

            TextField {
                id: textField
                readOnly: !activeFocus

                text: styleData.value

                FontLoader { id: firaSansSemiBold; source: "../../fonts/FiraSans-SemiBold.ttf"; }
                font.family: firaSansSemiBold.name
                font.pixelSize: hifi.fontSizes.textFieldInput
                height: hifi.dimensions.tableRowHeight

                style: TextFieldStyle {
                    textColor: readOnly
                               ? hifi.colors.black
                               : (treeView.isLightColorScheme ?  hifi.colors.black :  hifi.colors.white)
                    background: Rectangle {
                        visible: !readOnly
                        color: treeView.isLightColorScheme ? hifi.colors.white : hifi.colors.black
                        border.color: hifi.colors.primaryHighlight
                        border.width: 1
                    }
                    selectedTextColor: hifi.colors.black
                    selectionColor: hifi.colors.primaryHighlight
                    padding.left: readOnly ? 0 : hifi.dimensions.textPadding
                    padding.right: readOnly ? 0 : hifi.dimensions.textPadding
                }

                validator: RegExpValidator {
                    regExp: /[^/]+/
                }

                Keys.onPressed: {
                    if (event.key == Qt.Key_Escape) {
                        text = styleData.value;
                        unfocusHelper.forceActiveFocus();
                        event.accepted = true;
                    }
                }
                onAccepted:  {
                    if (acceptableInput && styleData.selected) {
                        if (!modifyEl(selection.currentIndex, text)) {
                            text = styleData.value;
                        }
                        unfocusHelper.forceActiveFocus();
                    }
                }

                onReadOnlyChanged: {
                    // Have to explicily set keyboardRaised because automatic setting fails because readOnly is true at the time.
                    keyboardRaised = activeFocus;
                }
            }
        }
    }

    Item {
        id: unfocusHelper
        visible: false
    }

    onDoubleClicked: isExpanded(index) ? collapse(index) : expand(index)

    onActivated: {
        var path = scriptsModel.data(index, 0x100)
        if (path) {
            loadScript(path)
        }
    }
}
