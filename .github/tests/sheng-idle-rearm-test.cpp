/*
 * SPDX-License-Identifier: MIT
 *
 * Runtime integration test for the Wayland KIdleTime poller. The stock
 * 6.13.0 plugin reaches the first timeout but its simulateUserActivity()
 * implementation is empty, so neither the resume signal nor the second
 * timeout occurs. The patched plugin must complete the full sequence.
 */

#include <KIdleTime>

#include <QCoreApplication>
#include <QGuiApplication>
#include <QLoggingCategory>
#include <QTimer>

int main(int argc, char **argv)
{
    QGuiApplication app(argc, argv);

    const QByteArray pluginRoot = qgetenv("SHENG_IDLE_PLUGIN_ROOT");
    if (!pluginRoot.isEmpty()) {
        QCoreApplication::addLibraryPath(QString::fromLocal8Bit(pluginRoot));
    }

    qInfo().noquote() << "SHENG_TEST library_paths=" << QCoreApplication::libraryPaths().join(QLatin1Char(':'));

    KIdleTime *idle = KIdleTime::instance();
    int timeoutCount = 0;
    int resumeCount = 0;

    QObject::connect(idle, &KIdleTime::resumingFromIdle, &app, [&] {
        ++resumeCount;
        qInfo() << "SHENG_TEST resume" << resumeCount;
    });

    QObject::connect(idle,
                     qOverload<int, int>(&KIdleTime::timeoutReached),
                     &app,
                     [&](int identifier, int milliseconds) {
                         Q_UNUSED(identifier);
                         if (milliseconds != 3000) {
                             return;
                         }

                         ++timeoutCount;
                         qInfo() << "SHENG_TEST timeout" << timeoutCount;

                         if (timeoutCount == 1) {
                             idle->catchNextResumeEvent();
                             QTimer::singleShot(0, idle, [idle] {
                                 qInfo() << "SHENG_TEST simulate";
                                 idle->simulateUserActivity();
                             });
                         } else if (timeoutCount == 2 && resumeCount == 1) {
                             qInfo() << "SHENG_TEST PASS";
                             app.exit(0);
                         }
                     });

    idle->addIdleTimeout(3000);

    QTimer::singleShot(15000, &app, [&] {
        qCritical() << "SHENG_TEST FAIL timeout_count=" << timeoutCount << "resume_count=" << resumeCount;
        app.exit(2);
    });

    return app.exec();
}
