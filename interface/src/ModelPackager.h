//
//  ModelPackager.h
//
//
//  Created by Clement on 3/9/15.
//  Copyright 2015 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//

#ifndef hifi_ModelPackager_h
#define hifi_ModelPackager_h

#include <memory>

#include <QFileInfo>
#include <QVariantHash>

#include "ui/ModelsBrowser.h"

class HFMModel;

class ModelPackager : public QObject {
public:
    static bool package();
    
private:
    bool selectModel();
    
    bool loadModel();
    bool editProperties();
    bool zipModel();
    
    void populateBasicMapping(QVariantHash& mapping, QString filename, const HFMModel& hfmModel);
    
    void listTextures();
    bool copyTextures(const QString& oldDir, const QDir& newDir);
    
    QFileInfo _modelFile;
    QFileInfo _fbxInfo;
    FSTReader::ModelType _modelType;
    QString _texDir;
    QString _scriptDir;

    QVariantHash _mapping;
    std::unique_ptr<HFMModel> _hfmModel;
    QStringList _textures;
    QStringList _scripts;
};




#endif // hifi_ModelPackager_h
