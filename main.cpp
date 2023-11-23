#include <QCoreApplication>
#include <QDebug>
#include "dtranms_proto.h"
#include "trafficgenerator.h"
#include <sys/time.h>
#include <time.h>

bool verbose = false;
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QString host;
    uint16_t port_src = 0, port_dst = 0;
    uint8_t SID = 0;
    uint8_t fec = DTRANMS::fec1_2;
    uint16_t size = 32;
    uint32_t timeout = 1000;
    uint32_t count = 0;
    uint32_t target = 0;
    bool group = false;

    QStringList arg = a.arguments();
    if (arg.count() < 2)
        goto usage;

    for (auto i = arg.constBegin() + 1; i != arg.constEnd(); i++) {
        if ((*i) == "--help") {
            goto usage;
        } else if ((*i) == "-h") {
            ++i;
            host = *i;
            continue;
        } else if ((*i) == "-ps") {
            ++i;
            port_src = (*i).toUInt();
            continue;
        } else if ((*i) == "-pd") {
            ++i;
            port_dst = (*i).toUInt();
            continue;
        } else if ((*i) == "-sid") {
            ++i;
            SID = (*i).toUInt();
            continue;
        } else if ((*i) == "-fec") {
            ++i;
            fec = (*i).toUInt();
            if (fec > 2) {
                printf("FEC is incorrect value\n", qPrintable(*i));
                goto usage;
            }
            continue;
        } else if ((*i) == "-l") {
            ++i;
            size = (*i).toUInt();
            if (size > 1500) {
                printf("Size is incorrect value\n", qPrintable(*i));
                goto usage;
            }
            continue;
        } else if ((*i) == "-w") {
            ++i;
            timeout = (*i).toUInt();
            continue;
        } else if ((*i) == "-n") {
            ++i;
            count = (*i).toUInt();
            continue;
        } else if ((*i) == "-g") {
            group = true;
            continue;
        } else if ((*i) == "-v") {
            verbose = true;
            continue;
        } else {
            if ((*i)[0] != '-') {
                target = (*i).toUInt();
                if ((target < 1) || (target > 16776415)) {
                    printf("Target is incorrect value\n", qPrintable(*i));
                    goto usage;
                }
            } else {
                printf("Unknown argument `%s`\n", qPrintable(*i));
                goto usage;
            }
        }
    };

    if (host.isEmpty()) {
        printf("Host address is missing\n");
        goto usage;
    }
    if (!port_src) {
        printf("Source UDP port is missing\n");
        goto usage;
    }
    if (!port_dst) {
        printf("Target UDP port is missing\n");
        goto usage;
    }

    try {
        TrafficGenerator
            gen(&a, host, port_src, port_dst, SID, (DTRANMS::eFEC) fec, size, timeout, count);
        gen.start(target, group);
        a.exec();
    } catch (std::exception const &e) {
        printf("Exception: %s\n", e.what());
    }

    return 0;

usage:
    printf("Usage: demo-dtranpulsar_ms -h host -ps udp_port_src -pd udp_port_dest -sid SID\n"
           "                           [-fec FEC] [-l size]\n"
           "                           [-w timeout] [-n count] [-v] [-g] dmr_target\n"
           "dmr_target - DMR number of target (1-16776415)\n"
           "Options:\n"
           "\t-h\t\tHost gateway address\n"
           "\t-ps\t\tSource UDP port\n"
           "\t-pd\t\tDestination UDP port\n"
           "\t-sid\t\tService identificator [0...255]\n"
           "\t-fec\t\tSelect Forward Error Correction:\n"
           "\t\t\t\t0-Without correction\n"
           "\t\t\t\t1-Rate 3/4 Trellis\n"
           "\t\t\t\t2-Rate 1/2 Block Product Turbo Codes (default)\n"
           "\t-l\t\tPacket size [0...1500], default=32\n"
           "\t-w\t\tTimeout between packets [ms], default=1000 ms, 0-timeout depends on bandwidth\n"
           "\t-n\t\tCount packets, default=0 (infinitely)\n"
           "\t-v\t\tVerbose exchange\n"
           "\t-g\t\tTarget is DMR group\n");

    return 0;
}

void print(const char *format, va_list arglist)
{
    static char buf[4096];
    struct timeval tv;
    struct timezone tz;
    gettimeofday(&tv, &tz);
    int index = strftime(buf, sizeof(buf), "[%Y.%m.%d %H:%M:%S", localtime(&tv.tv_sec));

    index += snprintf(buf + index, sizeof(buf) - index, ",%03lu] ", tv.tv_usec / 1000);
    index += vsnprintf(buf + index, sizeof(buf) - index, format, arglist);

    va_end(arglist);

    fprintf(stderr, "%s\n", buf);
}
void log(const char *format, ...)
{
    if (!verbose)
        return;
    va_list arglist;
    va_start(arglist, format);
    print(format, arglist);
    va_end(arglist);
}
void info(const char *format, ...)
{
    va_list arglist;
    va_start(arglist, format);
    print(format, arglist);
    va_end(arglist);
}

uint64_t timestamp_ms()
{
#define NSEC_IN_MSEC 1000000
#define MSEC_IN_SEC 1000

    struct timespec tm;
    clock_gettime(CLOCK_MONOTONIC, &tm);
    return tm.tv_sec * MSEC_IN_SEC + (tm.tv_nsec / NSEC_IN_MSEC);
}
