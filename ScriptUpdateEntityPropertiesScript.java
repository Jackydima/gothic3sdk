package de.george.g3dit.scripts;

import java.util.ArrayList;
import java.util.List;
import java.util.Set;
import java.util.logging.Logger;
import java.util.stream.Collectors;

import com.google.common.base.Splitter;
import com.teamunify.i18n.I;

import de.george.g3dit.settings.LambdaOption;
import de.george.g3dit.settings.Option;
import de.george.g3dit.settings.OptionPanel;
import de.george.g3dit.settings.TextFieldOptionHandler;
import de.george.lrentnode.archive.eCEntity;
import de.george.lrentnode.classes.desc.CD;
import de.george.lrentnode.classes.desc.PropertyDescriptor;
import de.george.lrentnode.properties.*;

public class ScriptUpdateEntityPropertiesScript implements IScript {

	private static final Option<String> NAME = new LambdaOption<>("",
			(parent) -> new TextFieldOptionHandler(parent, I.tr("Name"), I.tr("Entity Name")),
			"UpdateEntityPropertiesScript.NAME", I.tr("Name"));

	private static final Option<String> NPCLEVEL = new LambdaOption<>("",
			(parent) -> new TextFieldOptionHandler(parent, I.tr("Level"), I.tr("NPCs Level")),
			"UpdateEntityPropertiesScript.NPCLEVEL", I.tr("Level"));

	private static final Option<String> NPCLEVELMAX = new LambdaOption<>("",
			(parent) -> new TextFieldOptionHandler(parent, I.tr("LevelMax"), I.tr("NPCs LevelMax")),
			"UpdateEntityPropertiesScript.NPCLEVELMAX", I.tr("LevelMax"));

	private static final Option<String> TREASURESET_1 = new LambdaOption<>("",
			(parent) -> new TextFieldOptionHandler(parent, I.tr("TS1"), I.tr("NPCs TS1")),
			"UpdateEntityPropertiesScript.TREASURESET_1", I.tr("TS1"));

	private static final Option<String> TREASURESET_2 = new LambdaOption<>("",
			(parent) -> new TextFieldOptionHandler(parent, I.tr("TS2"), I.tr("NPCs TS2")),
			"UpdateEntityPropertiesScript.TREASURESET_2", I.tr("TS2"));

	private static final Option<String> TREASURESET_3 = new LambdaOption<>("",
			(parent) -> new TextFieldOptionHandler(parent, I.tr("TS3"), I.tr("NPCs TS3")),
			"UpdateEntityPropertiesScript.TREASURESET_3", I.tr("TS1"));

	private static final Option<String> TREASURESET_4 = new LambdaOption<>("",
			(parent) -> new TextFieldOptionHandler(parent, I.tr("TS4"), I.tr("NPCs TS4")),
			"UpdateEntityPropertiesScript.TREASURESET_4", I.tr("TS4"));

	private static final Option<String> TREASURESET_5 = new LambdaOption<>("",
			(parent) -> new TextFieldOptionHandler(parent, I.tr("TS5"), I.tr("NPCs TS5")),
			"UpdateEntityPropertiesScript.TREASURESET_5", I.tr("TS5"));

	@Override
	public String getTitle() {
		return I.tr("Update Entity Level & TreasureSets");
	}

	@Override
	public String getDescription() {
		return I.tr("Update Entity Level & TreasureSets");
	}

	@Override
	public boolean execute(IScriptEnvironment env) {
		Set<String> Names = Splitter.on(',').omitEmptyStrings().trimResults().splitToStream(env.getOption(NAME))
				.collect(Collectors.toSet());

		return ScriptUtils.processAndSaveWorldFiles(env, (archive, file) -> {
			List<eCEntity> toBeUpdated = new ArrayList<>();
			for (eCEntity entity : archive.getEntities()) {
				if (entity.hasClass(CD.gCNPC_PS.class) && Names.contains(entity.getName())) {
					toBeUpdated.add(entity);
				}
			}

			gLong lLevel;
			gLong lLevelMax;

			try {
				lLevel = new gLong(Integer.parseInt(env.getOption(NPCLEVEL)));
			} catch (NumberFormatException e) {
				lLevel = new gLong(-1);
			}

			try {
				lLevelMax = new gLong(Integer.parseInt(env.getOption(NPCLEVELMAX)));
			} catch (NumberFormatException e) {
				lLevelMax = new gLong(-1);
			}

			final gLong clLevel = lLevel;
			final gLong clLevelMax = lLevelMax;

			toBeUpdated.forEach(e -> {
				if (clLevel.getLong() != -1) {
					e.getClass(CD.gCNPC_PS.class).setPropertyData(CD.gCNPC_PS.Level, clLevel);
				}

				if (clLevelMax.getLong() != -1) {
					e.getClass(CD.gCNPC_PS.class).setPropertyData(CD.gCNPC_PS.LevelMax, clLevelMax);
				}

				if (!env.getOption(TREASURESET_1).isEmpty()) {
					e.getClass(CD.gCInventory_PS.class).setPropertyData(CD.gCInventory_PS.TreasureSet1,
							new bCString(env.getOption(TREASURESET_1)));
				}
				if (!env.getOption(TREASURESET_2).isEmpty()) {
					e.getClass(CD.gCInventory_PS.class).setPropertyData(CD.gCInventory_PS.TreasureSet2,
							new bCString(env.getOption(TREASURESET_2)));
				}
				if (!env.getOption(TREASURESET_3).isEmpty()) {
					e.getClass(CD.gCInventory_PS.class).setPropertyData(CD.gCInventory_PS.TreasureSet3,
							new bCString(env.getOption(TREASURESET_3)));
				}
				if (!env.getOption(TREASURESET_4).isEmpty()) {
					e.getClass(CD.gCInventory_PS.class).setPropertyData(CD.gCInventory_PS.TreasureSet4,
							new bCString(env.getOption(TREASURESET_4)));
				}
				if (!env.getOption(TREASURESET_5).isEmpty()) {
					e.getClass(CD.gCInventory_PS.class).setPropertyData(CD.gCInventory_PS.TreasureSet5,
							new bCString(env.getOption(TREASURESET_5)));
				}
			});
			return toBeUpdated.size();
		}, I.tr("A total of {0} NPCs were updated."));
	}

	@Override
	public void installOptions(OptionPanel optionPanel) {
		optionPanel.addOption(NAME);
		optionPanel.addOption(NPCLEVEL);
		optionPanel.addOption(NPCLEVELMAX);
		optionPanel.addOption(TREASURESET_1);
		optionPanel.addOption(TREASURESET_2);
		optionPanel.addOption(TREASURESET_3);
		optionPanel.addOption(TREASURESET_4);
		optionPanel.addOption(TREASURESET_5);
	}
}