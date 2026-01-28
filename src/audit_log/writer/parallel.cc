/*
 * ModSecurity, http://www.modsecurity.org/
 * Copyright (c) 2015 - 2021 Trustwave Holdings, Inc. (http://www.trustwave.com/)
 *
 * You may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * If any of the files related to licensing are missing or if you have any
 * other questions related to licensing please contact Trustwave Holdings, Inc.
 * directly using the email address security@modsecurity.org.
 *
 */

#include "src/audit_log/writer/parallel.h"

#include <time.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#ifndef WIN32
#include <unistd.h>
#else
#include <io.h>
#include "src/compat/msvc.h"
#endif
#include <stdlib.h>

#include <fstream>
#include <mutex>
#include <vector>

#include "modsecurity/audit_log.h"
#include "modsecurity/transaction.h"
#include "src/utils/system.h"
#include "src/utils/md5.h"


namespace modsecurity {
namespace audit_log {
namespace writer {

namespace {

class ScopedFd {
 public:
    explicit ScopedFd(int fd) : m_fd(fd) { }
    ~ScopedFd() {
        if (m_fd >= 0) {
#ifndef WIN32
            close(m_fd);
#else
            _close(m_fd);
#endif
        }
    }
    ScopedFd(const ScopedFd&) = delete;
    ScopedFd& operator=(const ScopedFd&) = delete;

    int get() const {
        return m_fd;
    }

    int release() {
        int fd = m_fd;
        m_fd = -1;
        return fd;
    }

 private:
    int m_fd;
};

#ifndef WIN32
bool isSymlinkOrNotDirectory(const std::string &path, std::string *error) {
    struct stat st {};
    if (lstat(path.c_str(), &st) != 0) {
        error->assign("Not able to stat directory: " + path + ": " \
            + strerror(errno) + ".");
        return true;
    }
    if (S_ISLNK(st.st_mode)) {
        error->assign("Refusing to use symlinked directory: " + path);
        return true;
    }
    if (!S_ISDIR(st.st_mode)) {
        error->assign("Not a directory: " + path);
        return true;
    }
    return false;
}

bool ensureDirectory(const std::string &path, int mode, std::string *error) {
    if (path.empty()) {
        return true;
    }
    if (mkdir(path.c_str(), mode) == 0) {
        return true;
    }
    if (errno == EEXIST) {
        return !isSymlinkOrNotDirectory(path, error);
    }
    error->assign("Not able to create directory: " + path + ": " \
        + strerror(errno) + ".");
    return false;
}
#endif

bool ensureDirectories(const std::string &path, int mode, std::string *error) {
    if (path.empty()) {
        return true;
    }
    std::string current;
    size_t i = 0;
    if (path[0] == '/') {
        current = "/";
        i = 1;
    }
    while (i <= path.size()) {
        size_t next = path.find('/', i);
        if (next == std::string::npos) {
            next = path.size();
        }
        std::string segment = path.substr(i, next - i);
        if (!segment.empty()) {
            if (current.size() > 1 && current.back() != '/') {
                current.append("/");
            }
            current.append(segment);
#ifndef WIN32
            if (!ensureDirectory(current, mode, error)) {
                return false;
            }
#else
            if (!utils::createDir(current, mode, error)) {
                return false;
            }
#endif
        }
        i = next + 1;
    }
    return true;
}

std::string normalizeBasePath(const std::string &path) {
    if (path.empty()) {
        return path;
    }
    std::string normalized = path;
    while (normalized.size() > 1 && normalized.back() == '/') {
        normalized.pop_back();
    }
    return normalized;
}

std::string buildDirectoryPath(const time_t *t,
    audit_log::AuditLog::AuditLogStorageDirMode mode) {
    struct tm timeinfo;
    localtime_r(t, &timeinfo);

    std::string dir;
    char tstr[std::size("/yyyymmdd-hhmm")];

    switch (mode) {
        case audit_log::AuditLog::StorageDirDay: {
            strftime(tstr, std::size(tstr), "/%Y%m%d", &timeinfo);
            dir.append(tstr);
            break;
        }
        case audit_log::AuditLog::StorageDirHour: {
            strftime(tstr, std::size(tstr), "/%Y%m%d", &timeinfo);
            dir.append(tstr);
            char hstr[std::size("/yyyymmdd-hh")];
            strftime(hstr, std::size(hstr), "/%Y%m%d-%H", &timeinfo);
            dir.append(hstr);
            break;
        }
        case audit_log::AuditLog::StorageDirMinute: {
            strftime(tstr, std::size(tstr), "/%Y%m%d", &timeinfo);
            dir.append(tstr);
            strftime(tstr, std::size(tstr), "/%Y%m%d-%H%M", &timeinfo);
            dir.append(tstr);
            break;
        }
        case audit_log::AuditLog::StorageDirSecond:
        default: {
            // Legacy layout: day + minute directories, second in file name.
            strftime(tstr, std::size(tstr), "/%Y%m%d", &timeinfo);
            dir.append(tstr);
            strftime(tstr, std::size(tstr), "/%Y%m%d-%H%M", &timeinfo);
            dir.append(tstr);
            break;
        }
    }
    return dir;
}

std::string buildFileName(const time_t *t) {
    struct tm timeinfo;
    localtime_r(t, &timeinfo);
    char tstr[std::size("/yyyymmdd-hhmmss")];
    strftime(tstr, std::size(tstr), "/%Y%m%d-%H%M%S", &timeinfo);
    return std::string(tstr);
}

#ifndef WIN32
bool secureWriteFile(const std::string &path, const std::string &data,
    int mode, std::string *error) {
    int flags = O_CREAT | O_EXCL | O_WRONLY | O_APPEND;
#ifdef O_NOFOLLOW
    flags |= O_NOFOLLOW;
#endif
    int fd = open(path.c_str(), flags, mode);
    if (fd < 0) {
#ifndef O_NOFOLLOW
        if (errno == EEXIST) {
            struct stat st {};
            if (lstat(path.c_str(), &st) == 0 && S_ISLNK(st.st_mode)) {
                error->assign("Refusing to write to symlinked file: " + path);
                return false;
            }
        }
#endif
        error->assign("Not able to open: " + path + ". " + strerror(errno));
        return false;
    }

    ScopedFd scoped(fd);
    size_t total = 0;
    while (total < data.size()) {
        ssize_t wrote = write(scoped.get(), data.data() + total, data.size() - total);
        if (wrote < 0) {
            error->assign("Failed to write: " + path + ". " + strerror(errno));
            return false;
        }
        total += static_cast<size_t>(wrote);
    }
    return true;
}
#endif

}  // namespace


Parallel::~Parallel() {
    utils::SharedFiles::getInstance().close(m_audit->m_path1);
    utils::SharedFiles::getInstance().close(m_audit->m_path2);
}


inline std::string Parallel::logFilePath(const time_t *t,
    int part) {
    std::string name;

    struct tm timeinfo;
    localtime_r(t, &timeinfo);

    if (part & YearMonthDayDirectory) {
        char tstr[std::size("/yyyymmdd")];
        strftime(tstr, std::size(tstr), "/%Y%m%d", &timeinfo);
        name.append(tstr);
    }

    if (part & YearMonthDayAndTimeDirectory) {
        char tstr[std::size("/yyyymmdd-hhmm")];
        strftime(tstr, std::size(tstr), "/%Y%m%d-%H%M", &timeinfo);
        name.append(tstr);
    }

    if (part & YearMonthDayAndTimeFileName) {
        char tstr[std::size("/yyyymmdd-hhmmss")];
        strftime(tstr, std::size(tstr), "/%Y%m%d-%H%M%S", &timeinfo);
        name.append(tstr);
    }

    return name;
}


bool Parallel::init(std::string *error) {
    bool ret;
    if (!m_audit->m_path1.empty()) {
        ret = utils::SharedFiles::getInstance().open(m_audit->m_path1, error);
        if (!ret) {
            return false;
        }
    }

    if (!m_audit->m_path2.empty()) {
        ret = utils::SharedFiles::getInstance().open(m_audit->m_path2, error);
        if (!ret) {
            return false;
        }
    }

    if (m_audit->m_storage_dir.empty() == false) {
        std::string base = normalizeBasePath(m_audit->m_storage_dir);
        if (!ensureDirectories(base, m_audit->getDirectoryPermission(), error)) {
            return false;
        }
    }

    return true;
}


bool Parallel::write(Transaction *transaction, int parts, std::string *error) {
    std::string log;
    std::string dirPath;
    std::string fileName;
    bool ret;

    if (transaction->m_rules->m_auditLog->m_format ==
            audit_log::AuditLog::JSONAuditLogFormat) {
        log = transaction->toJSON(parts);
    } else {
        std::string boundary;
        generateBoundary(&boundary);
        log = transaction->toOldAuditLogFormat(parts, "-" + boundary + "--", m_audit->m_prefix);
    }

    const auto &logPath = m_audit->m_storage_dir;
    if (logPath.empty()) {
        error->assign("Log path is not valid.");
        return false;
    }

    dirPath = buildDirectoryPath(&transaction->m_timeStamp,
        m_audit->getStorageDirModeType());
    std::string base = normalizeBasePath(logPath);
    std::string fullDirPath = base + dirPath;
    std::string fileLeaf = buildFileName(&transaction->m_timeStamp);
    fileName = fullDirPath + fileLeaf + "-" + transaction->m_id;

    ret = ensureDirectories(fullDirPath, m_audit->getDirectoryPermission(), error);
    if (ret == false) {
        return false;
    }
#ifndef WIN32
    if (!secureWriteFile(fileName, log, m_audit->getFilePermission(), error)) {
        return false;
    }
#else
    std::ofstream myfile;
    std::string a(fileName.c_str());
    myfile.open(a, std::ofstream::out | std::ofstream::app);
    if (!myfile.is_open()) {
        error->assign("Not able to open: " + fileName + ". " + strerror(errno));
        return false;
    }
    myfile << log;
    if (myfile.fail()) {
        error->assign("Failed to write: " + fileName + ". " + strerror(errno));
        return false;
    }
    myfile.close();
#endif

    if (m_audit->m_path1.empty() == false
        && m_audit->m_path2.empty() == false) {
        std::string msg = transaction->toOldAuditLogFormatIndex(fileName,
            log.length(), Utils::Md5::hexdigest(log));
        ret = utils::SharedFiles::getInstance().write(m_audit->m_path2, msg,
            error);
        if (ret == false) {
            return false;
        }
    }
    if (m_audit->m_path1.empty() == false
        && m_audit->m_path2.empty() == true) {
        std::string msg = transaction->toOldAuditLogFormatIndex(fileName,
            log.length(), Utils::Md5::hexdigest(log));
        ret = utils::SharedFiles::getInstance().write(m_audit->m_path1, msg,
            error);
        if (ret == false) {
            return false;
        }
    }
    if (m_audit->m_path1.empty() == true
        && m_audit->m_path2.empty() == false) {
        std::string msg = transaction->toOldAuditLogFormatIndex(fileName,
            log.length(), Utils::Md5::hexdigest(log));
        ret = utils::SharedFiles::getInstance().write(m_audit->m_path2, msg,
            error);
        if (ret == false) {
            return false;
        }
    }

    return true;
}

}  // namespace writer
}  // namespace audit_log
}  // namespace modsecurity
