#include "StdAfx.h"
#include "ObjectOpener.h"
#include "Tunnel_Base.h"
#include "TunnelNode.h"
#include "TunnelTag.h"
#include "TunnelReactor.h"

using namespace MIM;

static void handleTunnelNode(int i, TunnelNode* otherNode, const Tunnel_Base* tunnel);

static void modifyNode(const Tunnel_Base* pTunnel);
template<typename T>
static void handleTunnel(int i, Tunnel_Base* otherTunnel, const Tunnel_Base* tunnel);

ACRX_NO_CONS_DEFINE_MEMBERS(TunnelReactor, AcDbEntityReactor);

void TunnelReactor::erased(const AcDbObject* dbObj, Adesk::Boolean bErasing)
{
	if (bErasing == true)
	{
		Acad::ErrorStatus es = Acad::eOk;

		const Tunnel_Base *pTunnel = static_cast<const Tunnel_Base*>(dbObj);

		//�Լ��ľ��
		AcDbHandle handle;
		pTunnel->getAcDbHandle(handle);

		const std::vector<AcDbHandle>& nodesHandle = pTunnel->getNodesHandle();

		for (auto& nodeHandle : nodesHandle)
		{
			TunnelNode* Node = nullptr;
			ObjectOpener<TunnelNode> opener(Node, nodeHandle, AcDb::kForWrite);
			es = opener.open();
			if (es == Acad::eOk)
			{
				//�ӽڵ����Ƴ��Ը���������ü���
				Node->removeTunnel(handle);

				//�������Ϊ0��ɾ��
				if (bErasing == true)
				{
					bool isErased = Node->tryErase();
				}
			}

			//ɾ����ע
			const AcDbHandle& tagHandle = pTunnel->getTagHandle();
			TunnelTag* pTag = nullptr;
			ObjectOpener<TunnelTag> opener1(pTag, tagHandle, AcDb::kForWrite);
			if ((es = opener1.open()) == Acad::eOk)
			{
				pTag->canErase(true);
				pTag->erase(true);
			}
		}
	}
}

void TunnelReactor::modified(const AcDbObject* dbObj)
{
	//����ģʽ�²�ִ��
	if (Tunnel_Base::getAnimateMode() == true)
		return;
	//��ɾ���򼴽���ɾ��ʱ��ִ��
	if (dbObj->isEraseStatusToggled() || dbObj->isErased())
		return;

	Acad::ErrorStatus es = Acad::eOk;

	const Tunnel_Base *pTunnel = static_cast<const Tunnel_Base*>(dbObj);
	


	const std::vector<AcGePoint3d>& points = pTunnel->getBasePoints();
	const std::vector<UInt32>& colors = pTunnel->getColors();

	const std::vector<AcDbHandle>& nodeHandles = pTunnel->getNodesHandle();

	int i = 0;
	for (auto & handle : nodeHandles)
	{
		TunnelNode* pNode = nullptr;
		ObjectOpener<TunnelNode> opener(pNode, handle, AcDb::kForWrite);

		if ((es = opener.open()) == Acad::eOk)
		{			
			if(pNode->getPosition() != points.at(i))
			pNode->setPosition(points.at(i));
		}
		i++;
	}

	//�ƶ���ı��ע��λ��
	const AcDbHandle& tagHandle = pTunnel->getTagHandle();
	TunnelTag* pTag = nullptr;
	ObjectOpener<TunnelTag> opener(pTag, tagHandle, AcDb::kForWrite);
	if ((es = opener.open()) == Acad::eOk)
	{
		auto av = points.at(points.size() / 2 - 1) - points.at(points.size() / 2);
		auto newStdPoint = points.at(points.size() / 2) + av / 2;
		auto oldStdPoint = pTag->getStartPoint();

		auto tr = newStdPoint - oldStdPoint;
		AcGeMatrix3d xform;
		xform.setToTranslation(tr);
		pTag->transformBy(xform);

		pTag->setText(pTunnel->getTagData());
	}
}