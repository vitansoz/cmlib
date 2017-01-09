import QtQuick 1.1
import org.tal.cm 1.0

Rectangle {
    id: root
    width: 600
    height: 600

    CMMediaPlayer {
        id: player
        Component.onCompleted: setAudioSink(audioSink)

        onMetadata: {
            console.debug(meta.title)
        }
        onEot: {
            console.debug("EOT!")
            player.stop();
        }
    }

    CMAudioSink {
        id: audioSink
    }

    CMWavFileAudioSink {
        id: wavSink
        file: "audio-qml.wav"
    }

    Row {
        id: control
        width: parent.width
        anchors.top: parent.top
        Button {
            title: "Play"
            onClicked: {
                if (!player.play())
                    console.debug("Playback start failed")
            }
        }

        Button {
            title: "Pause"
            onClicked: player.pause();
        }

        Button {
            title: "Resume"
            onClicked: player.play();
        }

        Button {
            title: "Stop"
            onClicked: {
                player.stop();
            }
        }

        Button {
            title: "Save WAV"
            //enabled: player.s
            onClicked: {
                player.setAudioSink(wavSink)
            }
        }
        Button {
            title: "TestSink"
            onClicked: {
                player.setAudioSink(testSink)
            }
        }

        Text {
            id: posText
            width: 80
            height: parent.height
            text:player.position/1000;
            horizontalAlignment: Text.AlignHCenter
        }
    }

    Column {
        width: parent.width
        anchors.top: control.bottom
        anchors.bottom: parent.bottom
        ListView {
            id: files
            delegate: fileDelegate
            clip: true;
            width: parent.width
            height: parent.height
            model: _files

            //flickableDirection: Flickable.HorizontalAndVerticalFlick

            signal inputSelected(variant input);
            property int currentInput: -1;

            Component {
                id: fileDelegate
                Rectangle {
                    // color: model.input_id==inputs.currentInput ? "#e5e5e5" : "#ffffff";
                    width: parent.width;
                    height: txt.height;
                    MouseArea {
                        anchors.fill: parent;
                        Text {
                            id: txt
                            text: display;
                            anchors.margins: 16;
                            font.pixelSize: 18
                        }
                        onClicked: {
                            console.debug("File: "+model.display);                            
                            if (player.load(model.display))
                                player.prepare();
                        }
                    }
                }
            }
        }
    }

}
