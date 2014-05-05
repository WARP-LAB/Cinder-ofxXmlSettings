Cinder-ofxXmlSettings
=====================

A port of openFrameworks ofxXmlSettings to Cinder. All credits to oF.
Quick and dirty. 

Note that there is XML in libcinder http://libcinder.org/docs/v0.8.3/guide___xml_tree.html . This port is only intended for reusing some oF code.

```
void serialize(const ofAbstractParameter & parameter);
void deserialize(ofAbstractParameter & parameter);
```
methods are disabled.

Original oF example https://github.com/openframeworks/openFrameworks/tree/master/examples/addons/xmlSettingsExample/src ported and included. See warning in the example for saving the XML file - current implementation demands that file exists.

