/****************************************************************************
Copyright (c) 2013-2014 Chukong Technologies Inc.

http://www.cocos2d-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/

#include "ui/UIHelper.h"
#include "ui/UIWidget.h"
#include "ui/UILayoutComponent.h"

NS_CC_BEGIN

namespace ui {

    Widget* Helper::seekWidgetByTag(Widget* root, int tag)
    {
        if (!root)
        {
            return nullptr;
        }
        if (root->getTag() == tag)
        {
            return root;
        }
        const auto& arrayRootChildren = root->getChildren();
        ssize_t length = arrayRootChildren.size();
        for (ssize_t i=0;i<length;i++)
        {
            Widget* child = dynamic_cast<Widget*>(arrayRootChildren.at(i));
            if (child)
            {
                Widget* res = seekWidgetByTag(child,tag);
                if (res != nullptr)
                {
                    return res;
                }
            }
        }
        return nullptr;
    }
    
    Widget* Helper::seekWidgetByName(Widget* root, const std::string& name)
    {
        if (!root)
        {
            return nullptr;
        }
        if (root->getName() == name)
        {
            return root;
        }
        const auto& arrayRootChildren = root->getChildren();
        for (auto& subWidget : arrayRootChildren)
        {
            Widget* child = dynamic_cast<Widget*>(subWidget);
            if (child)
            {
                Widget* res = seekWidgetByName(child,name);
                if (res != nullptr)
                {
                    return res;
                }
            }
        }
        return nullptr;
    }
    
    /*temp action*/
    Widget* Helper::seekActionWidgetByActionTag(Widget* root, int tag)
    {
        if (!root)
        {
            return nullptr;
        }
        if (root->getActionTag() == tag)
        {
            return root;
        }
        const auto& arrayRootChildren = root->getChildren();
        for (auto& subWidget : arrayRootChildren)
        {
            Widget* child = dynamic_cast<Widget*>(subWidget);
            if (child)
            {
                Widget* res = seekActionWidgetByActionTag(child,tag);
                if (res != nullptr)
                {
                    return res;
                }
            }
        }
        return nullptr;
    }
    
    std::string Helper::getSubStringOfUTF8String(const std::string& str, std::string::size_type start, std::string::size_type length)
    {
        if (length==0)
        {
            return "";
        }
        std::string::size_type c, i, ix, q, min=std::string::npos, max=std::string::npos;
        for (q=0, i=0, ix=str.length(); i < ix; i++, q++)
        {
            if (q==start)
            {
                min = i;
            }
            if (q <= start+length || length==std::string::npos)
            {
                max = i;
            }
            
            c = (unsigned char) str[i];
            
            if      (c<=127) i+=0;
            else if ((c & 0xE0) == 0xC0) i+=1;
            else if ((c & 0xF0) == 0xE0) i+=2;
            else if ((c & 0xF8) == 0xF0) i+=3;
            else return "";//invalid utf8
        }
        if (q <= start+length || length == std::string::npos)
        {
            max = i;
        }
        if (min==std::string::npos || max==std::string::npos)
        {
            return "";
        }
        return str.substr(min,max);
    }
    
    void Helper::doLayout(cocos2d::Node *rootNode)
    {
        for(auto& node : rootNode->getChildren())
        {
            auto com = node->getComponent(__LAYOUT_COMPONENT_NAME);
            Node *parent = node->getParent();
            if (nullptr != com && nullptr != parent) {
                LayoutComponent* layoutComponent = (LayoutComponent*)com;
                
                Size parentContentSize = parent->getContentSize();
                Vec2 nodePosition = node->getPosition();
                LayoutComponent::ReferencePoint referencePoint = layoutComponent->getReferencePoint();
                Vec2 normalizedPosition = node->getNormalizedPosition();
                
                Mat4 additionalMatrix;
                switch (referencePoint) {
                    case LayoutComponent::ReferencePoint::TOP_LEFT:
                        additionalMatrix.translate(0, parentContentSize.height - nodePosition.y * 2, 0);
                        break;
                    case LayoutComponent::ReferencePoint::BOTTOM_RIGHT:
                        additionalMatrix.translate(parentContentSize.width - nodePosition.x * 2, 0, 0);
                        break;
                    case LayoutComponent::ReferencePoint::TOP_RIGHT:
                        additionalMatrix.translate(parentContentSize.width - nodePosition.x * 2, parentContentSize.height - nodePosition.y * 2, 0);
                        break;
                    default:
                        break;
                }
                node->setAdditionalTransform(&additionalMatrix);
                
                //apater content size
                bool isUsingPercentContentSize = layoutComponent->isUsingPercentContentSize();
                Vec2 percentContentSize = layoutComponent->getPercentContentSize();
                if (isUsingPercentContentSize) {
                    Size adaptedContentSize = Size(parentContentSize.width * percentContentSize.x, parentContentSize.height * percentContentSize.y);
                    node->setContentSize(adaptedContentSize);
                }
            }
            
            doLayout(node);
            
        }
    }
    
}

NS_CC_END
