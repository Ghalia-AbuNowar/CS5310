#ifndef _SCENEGRAPHTEXTRENDERER_H_
#define _SCENEGRAPHTEXTRENDERER_H_

#include "SGNodeVisitor.h"
#include "GroupNode.h"
#include "LeafNode.h"
#include "TransformNode.h"
#include "RotateTransform.h"
#include "ScaleTransform.h"
#include "TranslateTransform.h"
#include <iostream>
#include <vector>

using namespace std;
namespace sgraph {
    class SceneGraphTextRenderer : public SGNodeVisitor {
    private:
        int indent;
        string getIndentation() {
            return string(indent * 2, ' ');  // Two spaces per level
        }

    public:
        SceneGraphTextRenderer() : indent(0) {}

        void visitGroupNode(GroupNode *node) override {
            cout << getIndentation() << "- " << node->getName() << endl;
            indent++;  // Increase depth for children
            for (auto &child : node->getChildren()) {
                child->accept(this);
            }
            indent--;  // Restore depth after processing children
        }

        void visitLeafNode(LeafNode *node) override {
            cout << getIndentation() << "- " << node->getName() << endl;
        }

        void visitTransformNode(TransformNode *node) override {
            cout << getIndentation() << "- " << node->getName() << endl;
            indent++;
            if (!node->getChildren().empty()) {
                node->getChildren()[0]->accept(this);
            }
            indent--;
        }

        void visitScaleTransform(ScaleTransform *node) override {
            visitTransformNode(node);
        }

        void visitTranslateTransform(TranslateTransform *node) override {
            visitTransformNode(node);
        }

        void visitRotateTransform(RotateTransform *node) override {
            visitTransformNode(node);
        }
    };
}

#endif
