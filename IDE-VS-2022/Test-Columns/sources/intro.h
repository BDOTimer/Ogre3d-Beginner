///----------------------------------------------------------------------------|
/// "intro.h"
///----------------------------------------------------------------------------:
#ifndef INTRO_H
#define INTRO_H
#include "primitives.h"

///---------|
/// Models. |
///---------:
namespace mdl
{   
    using namespace Ogre;
    using namespace OgreBites;

    ///-----------------------------|
    /// Rand.                       |
    ///-----------------------------:
    struct Rand
    {   Rand(            ){   srand((unsigned)time(0));}
        Rand(unsigned sid){   srand(sid)              ;}
        int operator(    )(int range_min, int range_max) const
        {   return rand() % (range_max - range_min) + range_min;
        }
    };


    ///------------------------------------------------------------------------|
    /// 
    ///------------------------------------------------------------------------:
/*  class InstancingExample
    {
    private:
        Ogre::SceneManager* mSceneMgr;
        Ogre::InstanceManager* mInstanceManager;
        std::vector<Ogre::InstancedEntity*> mInstances;
    
    public:
        void setupInstancing()
        {
            // Загружаем меш один раз
            Ogre::MeshPtr mesh = Ogre::MeshManager::getSingleton().load(
                "robot.mesh",
                Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME
            );
        
            // Создаем менеджер инстансов
            // Параметры конструктора:
            // 1. Имя менеджера (должно быть уникальным)
            // 2. Имя меша
            // 3. Группа ресурсов
            // 4. Тип инстансинга
            // 5. Макс. инстансов в одном пакете
            // 6. Флаги
            // 7. Дополнительные данные
            mInstanceManager = mSceneMgr->createInstanceManager(
                "RobotInstanceManager",           // уникальное имя
                "robot.mesh",                    // используемый меш
                Ogre::ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME,
                Ogre::InstanceManager::HWInstancingBasic, // тип инстансинга
                200,                             // макс. инстансов в одном пакете (batch)
                Ogre::InstanceManager::IM_USEALL // флаги
            );
        
            // Альтернативные типы инстансинга:
            // - HWInstancingBasic: базовый аппаратный инстансинг
            // - HWInstancingVTF: инстансинг через текстуру (Vertex Texture Fetch)
            // - HWInstancingShaderBased: через шейдеры
        
            // Альтернативные флаги:
            // - IM_USEALL: использовать все возможности
            // - IM_USE16BIT: использовать 16-битные индексы
            // - IM_VTFBESTFIT: оптимизировать для VTF
            // - IM_VTFBONEMATRIXLOOKUP: для скелетной анимации
        
            // Настраиваем менеджер (опционально)
            mInstanceManager->setNumCustomParams(0); // дополнительные параметры
        /// mInstanceManager->setNumTextureUnits(1); // количество текстурных юнитов
        }
    
        void createInstancedEntities(int count)
        {
            // Материал должен поддерживать инстансинг
            Ogre::String materialName = "Robot/Instanced";
        
            // Проверяем существование материала
            if (!MaterialManager::getSingleton().resourceExists(materialName))
            {
                // Создаем простой материал программно, если нет файла
                createInstancedMaterial();
            }
        
            // Создаем инстансированные объекты
            mInstances.reserve(count);
        
            for (int i = 0; i < count; i++)
            {
                // Создаем инстансированную сущность
                Ogre::InstancedEntity* instancedEntity = mInstanceManager->createInstancedEntity(
                    materialName  // материал для инстансов
                );
            
                // Создаем SceneNode для позиционирования
                SceneNode* node
                    = mSceneMgr->getRootSceneNode()->createChildSceneNode();
            
                // Привязываем инстанс к ноде
                node->attachObject(instancedEntity);
            
                // Устанавливаем позицию, поворот и масштаб
                float x = (i % 10) * 150.0f;
                float z = (i / 10) * 150.0f;
                node->setPosition(x, 0.0f, z);
            
                // Случайный поворот и масштаб
                node->yaw(Ogre::Degree(Ogre::Math::RangeRandom(0, 360)));
                float scale = Ogre::Math::RangeRandom(0.5f, 1.5f);
                node->setScale(scale, scale, scale);
            
                // Сохраняем указатель для управления
                mInstances.push_back(instancedEntity);
            
            //  Настраиваем пользовательские параметры (если нужно)
            //  instancedEntity->setCustomParam(
            //      0, Ogre::Vector4(1, 0, 0, 1)); // цвет
            }
        
            Ogre::LogManager::getSingleton().logMessage(
                "Created " + Ogre::StringConverter::toString(count) + " instanced entities"
            );
        }
    
        void createInstancedMaterial()
        {
            // Создаем материал программно
            Ogre::MaterialPtr material = Ogre::MaterialManager::getSingleton().create(
                "Robot/Instanced",
                Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME
            );
        
            Ogre::Technique* tech = material->getTechnique(0);
            Ogre::Pass* pass = tech->getPass(0);
        
            // Включаем инстансинг
            pass->setInstancingEnabled(true);
        
            // Настраиваем свойства инстансинга
            Ogre::InstancingTechnique* instancingTech = pass->getInstancingTechnique();
            if (!instancingTech)
            {
                instancingTech = pass->createInstancingTechnique();
            }
        
            // Используем шейдеры для инстансинга
            instancingTech->setInstancingMode(Ogre::InstancingTechnique::ShaderBased);
        
            // Добавляем определение для матриц инстансов
            Ogre::RenderState* renderState = instancingTech->getRenderState();
            renderState->addInstancedAttribute("worldMatrix", 0, 3, Ogre::VET_FLOAT3);
        
            // Настраиваем шейдеры (упрощенный вариант)
            pass->setVertexProgram("Instancing/VP");
            pass->setFragmentProgram("Instancing/FP");
        }
    
        void update(float timeSinceLastFrame)
        {
            // Анимация инстансов - пример
            for (size_t i = 0; i < mInstances.size(); i++)
            {
                Ogre::InstancedEntity* instance = mInstances[i];
                Ogre::SceneNode* node = static_cast<Ogre::SceneNode*>(
                    instance->getParentNode());
            
                // Простая анимация - вращение
                node->yaw(Ogre::Degree(timeSinceLastFrame * 30.0f));
            
                // Плавающее движение
                float height = Ogre::Math::Sin(
                    timeSinceLastFrame * 2.0f + i) * 20.0f;

                node->setPosition(
                    node->getPosition().x, height, node->getPosition().z);
            }
        }
    
        void cleanup()
        {
            // Важно: удаляем в правильном порядке
        
            // 1. Удаляем все инстансированные объекты
            for (Ogre::InstancedEntity* instance : mInstances)
            {
                Ogre::SceneNode* node 
                    = static_cast<Ogre::SceneNode*>(instance->getParentNode());

                node->detachAllObjects();
                mSceneMgr->destroySceneNode(node);
            /// mInstanceManager->destroyInstancedEntity(instance);
            }
            mInstances.clear();
        
            // 2. Удаляем менеджер инстансов
            if (mInstanceManager)
            {
                mSceneMgr->destroyInstanceManager(mInstanceManager);
                mInstanceManager = nullptr;
            }
        }
    };
*/


#include <Ogre.h>
#include <OgreInstancedEntity.h>
#include <OgreInstanceManager.h>

class Intro
{
private:
    Ogre::SceneManager*                  mSceneMgr;
    Ogre::InstanceManager*        mInstanceManager;
    std::vector<SceneNode*>             mInstances;
    
public:
    bool setup1(){return true;}
    bool setup()
    {
        mSceneMgr = Glob::scnMgr;

        mSceneMgr->setShadowTechnique( SHADOWTYPE_TEXTURE_ADDITIVE_INTEGRATED );
        mSceneMgr->setShadowTextureSelfShadow( true );
        mSceneMgr->setShadowCasterRenderBackFaces( true );
        
        try
        {
            // 1. Загружаем меш
            Ogre::MeshPtr mesh = Ogre::MeshManager::getSingleton().load(
                "cube.mesh",
                Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME
            );
            
            // 2. Создаем InstanceManager (ЭТО ГЛАВНЫЙ КЛАСС ДЛЯ ИНСТАНСИНГА)
            // В Ogre 1.x используйте эту сигнатуру:
            mInstanceManager = mSceneMgr->createInstanceManager(
                "RobotInstanceMgr_",// + Ogre::StringConverter::toString(rand()),
                "cube.mesh",  // имя меша
                Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
                Ogre::InstanceManager::HWInstancingBasic,  // тип инстансинга
                80,    // максимальное количество в одном батче
                //Ogre::InstanceManager::IM_USEALL  // флаги
                InstanceManagerFlags::IM_USEALL
            );

            std::cout << "Intro::setup() пройден ...\n";
            
            // Дополнительные настройки (опционально)
            mInstanceManager->setNumCustomParams(0); // количество пользовательских параметров
        /// mInstanceManager->setNumTextureUnits(1); // количество текстурных юнитов
            
            createInstances(5);

            return true;
        }
        catch (Ogre::Exception& e)
        {
            Ogre::LogManager::getSingleton().logMessage(
                "Ошибка создания InstanceManager: " + e.getFullDescription()
            );
            return false;
        }
    }
    
    void createInstances(int count)
    {
        //createInstancedMaterial();

        Ogre::SceneNode* node 
            = mSceneMgr->getRootSceneNode();

        node->setPosition(0, -50, -200);
        
        for (int i = 0; i < count; i++)
        {
            try 
            {
                // 3. Создаем инстансированную сущность
                Ogre::InstancedEntity* ent
                    = mInstanceManager->createInstancedEntity(
                        //"BaseInstancedMaterial"
                        "Examples/Robot"
                );

                // 5. Устанавливаем позицию, поворот, масштаб
                Vector3    pos((i % 5) * 200.0f, 0, (i / 5) * 200.0f);
                Quaternion rot(Ogre::Degree(0), Ogre::Vector3::UNIT_Y);
                Vector3    scale(1, 1, 1);

                
                
                // 6. Настраиваем пользовательские параметры (если нужно)
                Ogre::Vector4 color(
                    Ogre::Math::RangeRandom(0.3f, 1.0f),
                    Ogre::Math::RangeRandom(0.3f, 1.0f),
                    Ogre::Math::RangeRandom(0.3f, 1.0f),
                    1.0f
                );
                //inst->setCustomParam(0, color);

                ent->setPosition   (pos);
                ent->setOrientation(rot);
                
                SceneNode* sceneNode = node->createChildSceneNode();

                sceneNode->attachObject (ent);
                sceneNode->setPosition   (pos);
                sceneNode->setOrientation(rot);

                mInstances.push_back(sceneNode);
                
            }
            catch (Ogre::Exception& e)
            {
                Ogre::LogManager::getSingleton().logMessage(
                    "Ошибка создания инстанса " + Ogre::StringConverter::toString(i) + 
                    ": " + e.getDescription()
                );
            }
        }
    }
    
    void createInstancedMaterial()
    {
        // Способ 1: Создать материал в .material файле:
        /*
        material BaseInstancedMaterial
        {
            technique
            {
                pass
                {
                    ambient 0.5 0.5 0.5
                    diffuse 0.8 0.8 0.8 1.0
                    specular 0.2 0.2 0.2 16
                    
                    // Инстансинг включается автоматически при использовании InstanceManager
                    // НЕ нужно вызывать setInstancingEnabled или createInstancingTechnique
                    
                    texture_unit
                    {
                        texture robot_diffuse.jpg
                    }
                }
            }
        }
        */
        
        /*/ Способ 2: Создать материал программно
        if (!MaterialManager::getSingleton().resourceExists("BaseInstancedMaterial"))
        {
            MaterialPtr material = MaterialManager::getSingleton().create(
                "BaseInstancedMaterial",
                Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME
            );
            
            Ogre::Technique* tech = material->createTechnique();
            Ogre::Pass* pass = tech->createPass();
            
            // Просто настраиваем обычные свойства материала
            pass->setAmbient(0.5f, 0.5f, 0.5f);
            pass->setDiffuse(0.8f, 0.8f, 0.8f, 1.0f);
            pass->setSpecular(0.2f, 0.2f, 0.2f, 1.0f);
            pass->setShininess(16.0f);
            
            // Добавляем текстуру
            Ogre::TextureUnitState* tex = pass->createTextureUnitState();
            tex->setTextureName("r2skin.jpg");
        }
        /*/
    }
};


    ///------------------------------------------------------------------------|
    /// 
    ///------------------------------------------------------------------------:
    struct  Intro1 : Glob
    {       Intro1()
            {   
            }

        void setup(SceneNode* base)
        {    tree (base);
        }

        void update()
        {   
        }
        
    private:
        Rand rrand;

        void tree(SceneNode* base = nullptr)
        {   
            if(base) return;

            // Загружаем меш один раз
            Ogre::MeshPtr mesh = Ogre::MeshManager::getSingleton().load(
                "robot.mesh",
                Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME
            );
            
            InstanceManager* instanceMgr = Glob::scnMgr->createInstanceManager(
                "Christmas_Tree",                   // Имя менеджера
                "12150_Christmas_Tree_V2_L2.mesh",
                ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME,
                InstanceManager::HWInstancingBasic, // Тип
                80,                                 // Макс инстансов
                IM_USEALL | IM_USE16BIT,
                80                                  // Зарезервированная память
            );

            /// Альтернативные типы инстансинга:
            /// - HWInstancingBasic: базовый аппаратный инстансинг
            /// - HWInstancingVTF: инстансинг через текстуру (Vertex Texture Fetch)
            /// - HWInstancingShaderBased: через шейдеры
        
            /// Альтернативные флаги:
            /// - IM_USEALL: использовать все возможности
            /// - IM_USE16BIT: использовать 16-битные индексы
            /// - IM_VTFBESTFIT: оптимизировать для VTF
            /// - IM_VTFBONEMATRIXLOOKUP: для скелетной анимации

            // Настраиваем менеджер (опционально)
            instanceMgr->setNumCustomParams(0); // дополнительные параметры
        /// instanceMgr->setNumTextureUnits(1); // количество текстурных юнитов

            for (int i = 0; i < 10; i++)
            {
                InstancedEntity* inst 
                    = instanceMgr->createInstancedEntity("MyMaterial");

                SceneNode* node
                    = scnMgr->getRootSceneNode()->createChildSceneNode();

                node->attachObject(inst);
                node->setPosition (rnd(), 0, rnd());
            }
        }

        float rnd()
        {   return (float)rrand(-3000, 3000);
        }
    };
}

#endif // INTRO_H

