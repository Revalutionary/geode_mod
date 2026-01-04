#include <Geode/modify/GJGarageLayer.hpp>
#include <Geode/Modify/PlayerObject.hpp>
#include <cue/PlayerIcon.hpp>

using namespace geode::prelude;

#include "MainPopupMenu.hpp"

class $modify(HGJGarageLayer, GJGarageLayer) {
    bool init() {
        if (!GJGarageLayer::init())
            return false;
		if (Mod::get()->getSettingValue<bool>("enabled")) {

			auto playerIcon = cue::PlayerIcon::create(IconType::Cube, GameManager::get()->getPlayerFrame(), GameManager::get()->m_playerColor, GameManager::get()->m_playerColor2, GameManager::get()->m_playerGlowColor);
			auto sprite = CircleButtonSprite::create(playerIcon);
			sprite->setID("all-cubes-menu-sprite"_spr);
			sprite->setScale(0.8f);
			playerIcon->setScale(0.85f);

			auto btn = CCMenuItemSpriteExtra::create(sprite, this, menu_selector(HGJGarageLayer::onButton));
			btn->setPosition(ccp(20.25f, 101.5f));
			btn->setID("all-cubes-menu-button"_spr);
			
			auto subMenu = this->getChildByID("shards-menu");
			subMenu->addChild(btn);
		}

        return true;
    }

	public:

		void onButton(CCObject* sender) {
			geode::log::debug("button pressed");
			auto mainMenuPopup = MainPopupMenu::create("Choose which gamemodes");
			mainMenuPopup->m_scene = this;
			mainMenuPopup->show();
		}
};

class $modify(HPlayerObject, PlayerObject) {
	// WIP
	struct Fields {
		cue::PlayerIcon* m_customSprite = nullptr;
	};

		bool init(int player, int ship, GJBaseGameLayer* gameLayer, CCLayer* layer, bool playLayer) {
			if (!PlayerObject::init(player, ship, gameLayer, layer, playLayer)) 
				return false;
			if (Mod::get()->getSettingValue<bool>("enabled")) {
				if (!PlayLayer::get() && !Mod::get()->getSettingValue<bool>("enabled-main-menu")) {
					goto end;
				}
				auto fields = m_fields.self();

				if (isShipCube || isBallCube || isUFOCube || isWaveCube || isRobotCube || isSpiderCube || isSwingCube) {
					fields->m_customSprite = cue::PlayerIcon::create(IconType::Cube, player, GameManager::get()->m_playerColor, GameManager::get()->m_playerColor2, GameManager::get()->m_playerGlowColor);
					fields->m_customSprite->setScale(1.f);
					fields->m_customSprite->setAnchorPoint(ccp(0.f, 0.f));
					fields->m_customSprite->setPosition(ccp(-16.5f, -16.5f));
					fields->m_customSprite->setID("all-cubes-icon-replacement"_spr);
					this->addChild(fields->m_customSprite, 20);
					fields->m_customSprite->setVisible(true);
				} else {
					m_mainLayer->getChildByID("gamemode-frame")->setVisible(true);
				}
			}
			end:
			return true;
		}

		void update(float dt) {
			PlayerObject::update(dt);

			if (Mod::get()->getSettingValue<bool>("enabled")) {

				m_mainLayer->getChildByID("gamemode-frame")->setVisible(false);

				auto fields = m_fields.self();
				bool m_isCube = !m_isShip && !m_isBird && !m_isBall && !m_isDart && !m_isRobot && !m_isSpider && !m_isSwing;

				if (!PlayLayer::get() && !Mod::get()->getSettingValue<bool>("enabled-main-menu")) {
					m_mainLayer->getChildByID("glow-frame")->setVisible(true);
					m_mainLayer->getChildByID("gamemode-frame")->setVisible(true);
					goto trueEnd;
				}

				if (isShipCube || isBallCube || isUFOCube || isWaveCube || isRobotCube || isSpiderCube || isSwingCube) {
					if (auto gamemodeFrame = m_mainLayer->getChildByID("gamemode-frame"))
						gamemodeFrame->setVisible(false);
						m_mainLayer->getChildByID("glow-frame")->setVisible(false);
				} else {
					goto end; // if nothing is enabled, skip the rest of the if statements and enable original gamemode sprite
				}

				if ((this->m_isShip && isShipCube) || (this->m_isBird && isUFOCube) && fields->m_customSprite) {	
					if (m_mainLayer->getChildByID("ship-frame")) {
						m_mainLayer->getChildByID("ship-frame")->setVisible(false);
					}
					if (!Mod::get()->getSettingValue<bool>("icon-rotation")) {
						this->setRotation(0.f);
					}
				} else if (this->m_isRobot && isRobotCube && fields->m_customSprite) {
					this->m_robotBatchNode->setVisible(false);
				} else if (this->m_isSpider && isSpiderCube && fields->m_customSprite) {
					this->m_spiderBatchNode->setVisible(false);
				} else if (this->m_isSwing && isSwingCube && fields->m_customSprite) {
					m_mainLayer->getChildByID("swing-bottom-boost")->setVisible(true);
					m_mainLayer->getChildByID("swing-middle-boost")->setVisible(true);
					m_mainLayer->getChildByID("swing-top-boost")->setVisible(true);
					if (!Mod::get()->getSettingValue<bool>("icon-rotation")) {
						this->setRotation(0.f);
					}
				} else if (this->m_isBall && isBallCube && fields->m_customSprite) {
					if (!Mod::get()->getSettingValue<bool>("icon-rotation")) {
						this->setRotation(0.f);
					}
				} else if (this->m_isDart && isWaveCube && fields->m_customSprite) {
					fields->m_customSprite->setScale(static_cast<float>(Mod::get()->getSettingValue<double>("wave-size")));
				} else if (m_isCube && fields->m_customSprite) {
					if (!Mod::get()->getSettingValue<bool>("icon-rotation")) {
						this->setRotation(0.f);
					}
				} else if (!fields->m_customSprite) {
					geode::log::error("Couldn't grab cube sprite");
				} else {
					goto end; // getting here implies that the user doesn't have the gamemode they are in enabled, meaning we must enable the og gamemode's sprite
				}
				fields->m_customSprite->setVisible(true);
				goto trueEnd; // skip nothing enabled function call
				end:
				m_mainLayer->getChildByID("glow-frame")->setVisible(true);
				m_mainLayer->getChildByID("gamemode-frame")->setVisible(true);
				fields->m_customSprite->setVisible(false);
				trueEnd:
				if (m_isPlatformer && !Mod::get()->getSettingValue<bool>("enabled-platformer")) {
					fields->m_customSprite->setVisible(false);
					m_mainLayer->getChildByID("glow-frame")->setVisible(true);
					m_mainLayer->getChildByID("gamemode-frame")->setVisible(true);
					if (m_isRobot && isRobotCube) {
						m_robotBatchNode->setVisible(true);
						m_mainLayer->getChildByID("gamemode-frame")->setVisible(false);
					} else if (m_isSpider && isSpiderCube) {
						m_spiderBatchNode->setVisible(true);
						m_mainLayer->getChildByID("gamemode-frame")->setVisible(false);
					} else if ((m_isShip && isShipCube) || (m_isBird && isUFOCube)) {
						m_mainLayer->getChildByID("ship-frame")->setVisible(true);
					}
				}
			}
		}

		void toggleDartMode(bool enable, bool noEffects) {
			PlayerObject::toggleDartMode(enable, noEffects);
			if (Mod::get()->getSettingValue<bool>("enabled")) {
				if (!PlayLayer::get() && !Mod::get()->getSettingValue<bool>("enabled-main-menu")) {
					m_mainLayer->getChildByID("glow-frame")->setVisible(true);
					m_mainLayer->getChildByID("gamemode-frame")->setVisible(true);
					goto end;
				}
				auto fields = m_fields.self();

				if (enable && isWaveCube) {
					fields->m_customSprite->setScale(static_cast<float>(Mod::get()->getSettingValue<double>("wave-size")));
					fields->m_customSprite->setPosition(ccp(-5.5f, -5.5f));
				} else if (!enable && isWaveCube) {
					fields->m_customSprite->setScale(1.f);
					fields->m_customSprite->setPosition(ccp(-16.5f, -16.5f));
				}
			}
			end:
		}

		void playDeathEffect() {
			if (Mod::get()->getSettingValue<bool>("enabled")){
				auto fields = m_fields.self();
				fields->m_customSprite->setVisible(false);
				m_mainLayer->getChildByID("gamemode-frame")->setVisible(true);
			}

			PlayerObject::playDeathEffect();
		}

		void playCompleteEffect(bool noEffects, bool instant) {		
			if (Mod::get()->getSettingValue<bool>("enabled")) {
				auto fields = m_fields.self();
				fields->m_customSprite->setVisible(false);
			}	
			PlayerObject::playCompleteEffect(noEffects, instant);
		}

};


/* 

		TODO:
	Features:
 - Make player gamemode trail to be a cube as well when needed
 - FlipY when upsidedown

*/


/*
Dear HJFod,

You thought I was done? No no no, you can't escape this pal. I coded this mod so horribly that your eyes will bleed for days wishing you never made geode. You gave the general GD public so much control over the game that you will live to regret it. Sitting on your death bed you will regret 3 things: 1. Why did I make geode... 2. Why can't I lose weight... 3. For what is a man? What has he got? Yeah, that is from my way, because it's my way or the highway pal. You think your word is absolute huh? You aren't Allah, don't even try. Shut down shop unc, you don't got ts. Making children and trapping them in Nodes for their entire lives until they one day die from starvation isn't a good business move and the police is catching onto you. You can't keep getting away with this.

With all disrespect,
Revalutionary

(I would've coded this entire thing in TürkçeKod to make your life harder (and "That's just cool") but I couldn't be bothered)
*/