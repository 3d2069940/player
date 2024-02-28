
#ifndef _PARSERBINDINGS_H_
#define _PARSERBINDINGS_H_

#include <QStringList>

#include <yaml-cpp/yaml.h>

namespace YAML {
        template<>
    struct convert<QStringList> {
        static Node encode (const QStringList &value) {
            Node node;
            foreach (const QString &string, value)
                node.push_back(string.toStdString());
            return node;
        }
        static bool decode (const Node &node, QStringList &value) {
            if (!node.IsSequence())
                return false;
            for (YAML::const_iterator it = node.begin(); it != node.end(); ++it)
                if (it->IsScalar())
                    value << QString::fromStdString(it->as<std::string>());
            return true;
        }
    };
    template<>
    struct convert<QString> {
        static Node encode (const QString &value) {
            return Node (value.toStdString());
        }
        static bool decode (const Node &node, QString &value) {
            if (!node.IsScalar())
                return false;
            value = QString::fromStdString(node.as<std::string>());
            return true;
        }
    };
};

#endif // _PARSERBINDINGS_H_
