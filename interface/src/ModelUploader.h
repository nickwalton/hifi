//
//  ModelUploader.h
//  interface/src
//
//  Created by Clément Brisset on 3/4/14.
//  Copyright 2014 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//

#ifndef hifi_ModelUploader_h
#define hifi_ModelUploader_h

#include <QDialog>
#include <QTimer>

class QFileInfo;
class QHttpMultiPart;
class QLineEdit;
class QProgressBar;
class QPushButton;

class FBXGeometry;

class ModelUploader : public QObject {
    Q_OBJECT
    
public:
    ModelUploader(bool isHead);
    ~ModelUploader();
    
public slots:
    void send();
    
private slots:
    void checkJSON(const QJsonObject& jsonResponse);
    void uploadUpdate(qint64 bytesSent, qint64 bytesTotal);
    void uploadSuccess(const QJsonObject& jsonResponse);
    void uploadFailed(QNetworkReply::NetworkError errorCode, const QString& errorString);
    void checkS3();
    void processCheck();
    
private:
    QString _url;
    int _lodCount;
    int _texturesCount;
    int _totalSize;
    bool _isHead;
    bool _readyToSend;
    
    QHttpMultiPart* _dataMultiPart;
    QNetworkAccessManager _networkAccessManager;
    
    int _numberOfChecks;
    QTimer _timer;
    
    QDialog* _progressDialog;
    QProgressBar* _progressBar;
    
    
    bool zip();
    bool addTextures(const QString& texdir, const FBXGeometry& geometry);
    bool addPart(const QString& path, const QString& name);
    bool addPart(const QFile& file, const QByteArray& contents, const QString& name);
};

/// A dialog that allows customization of various model properties.
class ModelPropertiesDialog : public QDialog {
    Q_OBJECT

public:
    ModelPropertiesDialog(bool isHead, const QVariantHash& originalMapping);

    QVariantHash getMapping() const;

private slots:
    void reset();
    void chooseTextureDirectory();

private:
    QVariantHash _originalMapping;
    QLineEdit* _name;
    QPushButton* _textureDirectory;
};

#endif // hifi_ModelUploader_h
