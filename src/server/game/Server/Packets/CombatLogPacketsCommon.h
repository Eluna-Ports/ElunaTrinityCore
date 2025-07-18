/*
 * This file is part of the TrinityCore Project. See AUTHORS file for Copyright information
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef TRINITYCORE_COMBAT_LOG_PACKETS_COMMON_H
#define TRINITYCORE_COMBAT_LOG_PACKETS_COMMON_H

#include "ObjectGuid.h"
#include "Packet.h"

class Spell;
class Unit;

namespace WorldPackets
{
    namespace Spells
    {
        struct SpellLogPowerData
        {
            SpellLogPowerData(int8 powerType, int32 amount, int32 cost) : PowerType(powerType), Amount(amount), Cost(cost) { }

            int8 PowerType = 0;
            int32 Amount = 0;
            int32 Cost = 0;
        };

        struct SpellCastLogData
        {
            int64 Health = 0;
            int32 AttackPower = 0;
            int32 SpellPower = 0;
            int32 Armor = 0;
            int32 Unknown_1105_1 = 0;
            int32 Unknown_1105_2 = 0;
            std::vector<SpellLogPowerData> PowerData;

            void Initialize(Unit const* unit);
            void Initialize(Spell const* spell);
        };

        struct ContentTuningParams
        {
            enum ContentTuningType : uint32
            {
                TYPE_CREATURE_TO_PLAYER_DAMAGE          = 1,
                TYPE_PLAYER_TO_CREATURE_DAMAGE          = 2,
                TYPE_CREATURE_TO_PLAYER_HEALING         = 3,
                TYPE_PLAYER_TO_CREATURE_HEALING         = 4,
                TYPE_CREATURE_TO_CREATURE_DAMAGE        = 5,
                TYPE_CREATURE_TO_CREATURE_HEALING       = 6,
                TYPE_PLAYER_TO_PLAYER_DAMAGE            = 7, // NYI
                TYPE_PLAYER_TO_PLAYER_HEALING           = 8,
            };

            enum ContentTuningFlags : uint32
            {
                NO_LEVEL_SCALING        = 0x1,
                NO_ITEM_LEVEL_SCALING   = 0x2
            };

            uint32 Type = 0;
            int16 PlayerLevelDelta = 0;
            float PlayerItemLevel = 0;
            float TargetItemLevel = 0;
            int32 ScalingHealthItemLevelCurveID = 0;
            int32 Unused1117 = 0;
            int32 ScalingHealthPrimaryStatCurveID = 0;
            uint8 TargetLevel = 0;
            uint8 Expansion = 0;
            int8 TargetScalingLevelDelta = 0;
            uint32 Flags = NO_LEVEL_SCALING | NO_ITEM_LEVEL_SCALING;
            int32 PlayerContentTuningID = 0;
            int32 TargetContentTuningID = 0;
            int32 TargetHealingContentTuningID = 0; // direct heal only, not periodic
            float PlayerPrimaryStatToExpectedRatio = 1.0f;

            template<class T, class U>
            bool GenerateDataForUnits(T* attacker, U* target);
        };

        struct SpellCastVisual
        {
            int32 SpellXSpellVisualID = 0;
            int32 ScriptVisualID = 0;
        };

        struct SpellSupportInfo
        {
            ObjectGuid Supporter;
            int32 SupportSpellID = 0;
            int32 AmountRaw = 0;
            float AmountPortion = 0.0f;
        };

        ByteBuffer& operator<<(ByteBuffer& data, SpellCastLogData const& spellCastLogData);
        ByteBuffer& operator<<(ByteBuffer& data, ContentTuningParams const& contentTuningParams);
        ByteBuffer& operator>>(ByteBuffer& data, SpellCastVisual& visual);
        ByteBuffer& operator<<(ByteBuffer& data, SpellCastVisual const& visual);
        ByteBuffer& operator<<(ByteBuffer& data, SpellSupportInfo const& supportInfo);
    }

    namespace CombatLog
    {
        class CombatLogServerPacket : public ServerPacket
        {
        public:
            CombatLogServerPacket(OpcodeServer opcode, size_t initialSize = 200, ConnectionType connection = CONNECTION_TYPE_DEFAULT)
                : ServerPacket(opcode, initialSize, connection), _fullLogPacket(opcode, initialSize, connection) { }

            WorldPacket const* GetFullLogPacket() const { return &_fullLogPacket; }
            WorldPacket const* GetBasicLogPacket() const { return &_worldPacket; }

            Spells::SpellCastLogData LogData;

        protected:
            template <ByteBufferNumeric T>
            void operator<<(T val)
            {
                _worldPacket << val;
                _fullLogPacket << val;
            }

            template <typename T> requires (!ByteBufferNumeric<T>)
            void operator<<(T const& val)
            {
                _worldPacket << val;
                _fullLogPacket << val;
            }

            void WriteLogDataBit()
            {
                _worldPacket.WriteBit(false);
                _fullLogPacket.WriteBit(true);
            }

            void FlushBits()
            {
                _worldPacket.FlushBits();
                _fullLogPacket.FlushBits();
            }

            ByteBuffer& WriteLogData();

            WorldPacket _fullLogPacket;
        };
    }
}

#endif // TRINITYCORE_COMBAT_LOG_PACKETS_COMMON_H
