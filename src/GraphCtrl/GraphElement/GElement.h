/***************************
@Author: Chunel
@Contact: chunel@foxmail.com
@File: GElement.h
@Time: 2021/6/1 10:13 下午
@Desc: 
***************************/

#ifndef CGRAPH_GELEMENT_H
#define CGRAPH_GELEMENT_H

#include <set>
#include <vector>
#include <string>
#include <atomic>
#include <algorithm>

#include "../GraphObject.h"
#include "../GraphParam/GParamInclude.h"
#include "../GraphAspect/GAspectInclude.h"
#include "../../UtilsCtrl/UtilsInclude.h"

CGRAPH_NAMESPACE_BEGIN

class GElement : public GraphObject {
public:
    /**
     * 获取name信息
     */
    const std::string& getName() const;

    /**
     * 获取session信息（全局唯一）
     */
    const std::string& getSession() const;

    /**
     * 获取参数信息，如果未找到，则返回nullptr
     * @tparam T
     * @param key
     * @return
     */
    template<typename T,
            std::enable_if_t<std::is_base_of<GParam, T>::value, int> = 0>
    T* getGParam(const std::string& key);

    /**
     * 创建param信息，如果过了，则直接返回ok
     * @tparam T
     * @param key
     * @return
     */
    template<typename T,
            std::enable_if_t<std::is_base_of<GParam, T>::value, int> = 0>
    CStatus createGParam(const std::string& key);

    /**
     * 实现添加切面的逻辑
     * @tparam TAspect
     * @tparam TParam
     * @param param
     * @return
     */
    template<typename TAspect, typename TParam = GAspectDefaultParam,
            std::enable_if_t<std::is_base_of<GAspect, TAspect>::value, int> = 0,
            std::enable_if_t<std::is_base_of<GAspectParam, TParam>::value, int> = 0>
    GElement* addGAspect(TParam* param = nullptr);

    /**
     * 添加依赖节点信息
     * @param dependElements
     * @return
     */
    CStatus addDependElements(const std::set<GElement *> &dependElements);


protected:
    /**
     * 构造函数
     */
    explicit GElement();

    /**
     * 析构函数
     */
    ~GElement() override;

    /**
     * 实现拷贝构造函数
     * @param element
     */
    GElement(const GElement& element);

    /**
     * 赋值构造函数
     * @param element
     * @return
     */
    GElement& operator=(const GElement& element);

    /**
     * run方法执行之前的执行函数
     * @return
     */
    virtual CStatus beforeRun();

    /**
     * run方法执行之后的执行函数
     * @return
     */
    virtual CStatus afterRun();

    /**
     * 设置name信息
     * @param name
     * @return
     */
    GElement* setName(const std::string& name);

    /**
     * 设置循环次数
     * @param loop
     * @return
     */
    GElement* setLoop(CSize loop = 1);

    /**
     * 判定element是否可以运行
     * 可执行的条件为：自身未被执行且依赖节点全部被执行
     * @return
     */
    [[nodiscard]] bool isRunnable() const;

    /**
     * 判定node是否可以和前面节点一起执行
     * @return
     */
    [[nodiscard]] bool isLinkable() const;

    /**
     * 线程池中的运行函数，依次执行beforeRun，run和afterRun方法，
     * 其中有任何返回值问题，则直接返回
     * @param isMock 是否真实执行run方法。默认执行的
     * @return
     */
    virtual CStatus process(bool isMock);

    /**
     * 执行切面逻辑
     * @param aspectType
     * @param curStatus
     * @return
     */
    CStatus doAspect(const GAspectType& aspectType,
                     const CStatus& curStatus = CStatus());

    /**
     * 设置element信息
     * @param dependElements
     * @param name
     * @param loop
     * @param paramManager
     * @param threadPool
     * @return
     */
    virtual CStatus setElementInfo(const std::set<GElement *> &dependElements,
                                   const std::string &name,
                                   CSize loop,
                                   GParamManagerPtr paramManager,
                                   UThreadPoolPtr threadPool);

    /**
     * 包含切面相关功能的函数，fat取自fatjar的意思
     * @param type
     * @return
     */
    CStatus fatProcessor(const CFunctionType& type);


protected:
    CBool done_ { false };                           // 判定被执行结束
    CBool is_init_ { false };                        // 是否初始化了
    CBool linkable_ { false };                       // 判定是否可以连通计算
    CSize loop_ { 1 };                               // 节点执行次数
    std::string name_;                               // 节点名称
    std::string session_;                            // 节点唯一id信息
    std::set<GElement *> run_before_;                // 被依赖的节点
    std::set<GElement *> dependence_;                // 依赖的节点信息
    std::atomic<int> left_depend_ { 0 };          // 当 left_depend_ 值为0的时候，即可以执行该node信息
    GParamManagerPtr param_manager_ { nullptr };     // 整体流程的参数管理类，所有pipeline中的所有节点共享
    GAspectManagerPtr aspect_manager_ { nullptr };   // 整体流程的切面管理类
    UThreadPoolPtr thread_pool_ { nullptr };         // 用于执行的线程池信息

    friend class GNode;
    friend class GCluster;
    friend class GRegion;
    friend class GCondition;
    friend class GElementManager;
    friend class GGroup;
    friend class GPipeline;
    friend class GAdapter;
    template<typename T> friend class GSingleton;
};

using GElementPtr = GElement *;
using GElementPtrArr = std::vector<GElementPtr>;
using GElementPtrSet = std::set<GElementPtr>;
using GElementPtrSetRef = GElementPtrSet &;

CGRAPH_NAMESPACE_END

#include "GElement.inl"

#endif //CGRAPH_GELEMENT_H
