#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickStyle>

#include "artworkcanvas.h"
#include "documentcontroller.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    QGuiApplication::setApplicationName(QStringLiteral("FrameWorks"));
    QGuiApplication::setOrganizationName(QStringLiteral("FrameWorks"));
    QQuickStyle::setStyle(QStringLiteral("Basic"));

    qmlRegisterType<DocumentController>("FrameWorks.Native", 1, 0, "DocumentController");
    qmlRegisterType<ArtworkCanvas>("FrameWorks.Native", 1, 0, "ArtworkCanvas");

    QQmlApplicationEngine engine;
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed,
                     &app, [] { QCoreApplication::exit(EXIT_FAILURE); },
                     Qt::QueuedConnection);
    engine.loadFromModule("FrameWorks", "Main");

    return app.exec();
}
