
#include "../../Flare.h"
#include "FlareOrbitalMenu.h"
#include "../../Game/FlareGame.h"
#include "../../Game/FlareGameTools.h"
#include "../../Player/FlareMenuManager.h"
#include "../../Player/FlarePlayerController.h"
#include "../../Spacecrafts/FlareSpacecraft.h"
#include "../Components/FlareSectorButton.h"


#define LOCTEXT_NAMESPACE "FlareOrbitalMenu"


/*----------------------------------------------------
	Construct
----------------------------------------------------*/

void SFlareOrbitalMenu::Construct(const FArguments& InArgs)
{
	// Data
	MenuManager = InArgs._MenuManager;
	const FFlareStyleCatalog& Theme = FFlareStyleSet::GetDefaultTheme();
	Game = MenuManager->GetPC()->GetGame();
	FastForwardPeriod = 0.5f;

	// Build structure
	ChildSlot
	.HAlign(HAlign_Fill)
	.VAlign(VAlign_Fill)
	[
		SNew(SVerticalBox)

		+ SVerticalBox::Slot()
		.AutoHeight()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Center)
		.Padding(Theme.ContentPadding)
		[
			SNew(SHorizontalBox)

			// Icon
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(SImage).Image(AFlareMenuManager::GetMenuIcon(EFlareMenu::MENU_Orbit))
			]

			// Title
			+ SHorizontalBox::Slot()
			.VAlign(VAlign_Center)
			.Padding(Theme.ContentPadding)
			[
				SNew(STextBlock)
				.TextStyle(&Theme.TitleFont)
				.Text(LOCTEXT("Orbital", "ORBITAL MAP"))
			]

			// Company
			+ SHorizontalBox::Slot()
			.HAlign(HAlign_Right)
			.VAlign(VAlign_Bottom)
			.Padding(Theme.TitleButtonPadding)
			[
				SNew(SFlareRoundButton)
				.Text(LOCTEXT("InspectCompany", "Company"))
				.HelpText(LOCTEXT("InspectCompanyInfo", "Inspect your company"))
				.Icon(AFlareMenuManager::GetMenuIcon(EFlareMenu::MENU_Company, true))
				.OnClicked(this, &SFlareOrbitalMenu::OnInspectCompany)
			]

			// Fleets
			+ SHorizontalBox::Slot()
			.HAlign(HAlign_Right)
			.VAlign(VAlign_Bottom)
			.Padding(Theme.TitleButtonPadding)
			.AutoWidth()
			[
				SNew(SFlareRoundButton)
				.Text(LOCTEXT("InspectFleets", "Fleets"))
				.HelpText(LOCTEXT("InspectFleetsInfo", "Inspect and manage your fleets"))
				.Icon(AFlareMenuManager::GetMenuIcon(EFlareMenu::MENU_Fleet, true))
				.OnClicked(this, &SFlareOrbitalMenu::OnInspectFleet)
			]

			// Leaderboard
			+ SHorizontalBox::Slot()
			.HAlign(HAlign_Right)
			.VAlign(VAlign_Bottom)
			.Padding(Theme.TitleButtonPadding)
			.AutoWidth()
			[
				SNew(SFlareRoundButton)
				.Text(LOCTEXT("Leaderboard", "Competitors"))
				.HelpText(LOCTEXT("LeaderboardInfo", "Take a closer look at all the companies"))
				.Icon(AFlareMenuManager::GetMenuIcon(EFlareMenu::MENU_Leaderboard, true))
				.OnClicked(this, &SFlareOrbitalMenu::OnOpenLeaderboard)
			]

			// Quit
			+ SHorizontalBox::Slot()
			.HAlign(HAlign_Right)
			.VAlign(VAlign_Bottom)
			.Padding(Theme.TitleButtonPadding)
			.AutoWidth()
			[
				SNew(SFlareRoundButton)
				.Text(LOCTEXT("SaveQuit", "Save and quit"))
				.HelpText(LOCTEXT("SaveQuitInfo", "Save the game and go back to the main menu"))
				.Icon(AFlareMenuManager::GetMenuIcon(EFlareMenu::MENU_Main, true))
				.OnClicked(this, &SFlareOrbitalMenu::OnMainMenu)
			]
		]

		// Separator
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(FMargin(200, 20))
		[
			SNew(SImage).Image(&Theme.SeparatorBrush)
		]

		// Buttons
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(Theme.ContentPadding)
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Top)
		[
			SNew(SHorizontalBox)

			// Fast forward
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(Theme.SmallContentPadding)
			[
				SAssignNew(FastForward, SFlareButton)
				.Width(4)
				.Toggle(true)
				.Text(this, &SFlareOrbitalMenu::GetFastForwardText)
				.Icon(this, &SFlareOrbitalMenu::GetFastForwardIcon)
				.OnClicked(this, &SFlareOrbitalMenu::OnFastForwardClicked)
				.IsDisabled(this, &SFlareOrbitalMenu::IsFastForwardDisabled)
				.HelpText(LOCTEXT("FastForwardInfo", "Wait and see - Travels, production, building will be accelerated."))
			]

			// Fly selected ship
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(Theme.SmallContentPadding)
			[
				SNew(SFlareButton)
				.Width(9)
				.Text(this, &SFlareOrbitalMenu::GetFlySelectedShipText)
				.HelpText(LOCTEXT("FlySelectedInfo", "Fly the currently selected ship"))
				.Icon(FFlareStyleSet::GetIcon("Travel"))
				.OnClicked(this, &SFlareOrbitalMenu::OnFlySelectedShipClicked)
				.IsDisabled(this, &SFlareOrbitalMenu::IsFlySelectedShipDisabled)
			]

			// Fly last flown
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(Theme.SmallContentPadding)
			[
				SNew(SFlareButton)
				.Width(9)
				.Text(this, &SFlareOrbitalMenu::GetFlyCurrentShipText)
				.HelpText(LOCTEXT("FlyCurrentInfo", "Fly the last flown ship"))
				.Icon(FFlareStyleSet::GetIcon("Travel"))
				.OnClicked(this, &SFlareOrbitalMenu::OnFlyCurrentShipClicked)
				.IsDisabled(this, &SFlareOrbitalMenu::IsFlyCurrentShipDisabled)
			]
		]

		// Planetarium
		+ SVerticalBox::Slot()
		.Padding(Theme.ContentPadding)
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		[
			SNew(SHorizontalBox)

			// Left column
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Fill)
			.HAlign(HAlign_Fill)
			[
				SNew(SVerticalBox)
					
				// Date
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(Theme.ContentPadding)
				[
					SNew(STextBlock)
					.TextStyle(&Theme.TextFont)
					.Text(this, &SFlareOrbitalMenu::GetDateText)
				]
			
				// Nema
				+ SVerticalBox::Slot()
				[
					SAssignNew(NemaBox, SFlarePlanetaryBox)
				]
			]

			// Main column 
			+ SHorizontalBox::Slot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			[
				SNew(SVerticalBox)
				
				// Top line
				+ SVerticalBox::Slot()
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				[
					SNew(SHorizontalBox)

					// Anka
					+ SHorizontalBox::Slot()
					[
						SAssignNew(AnkaBox, SFlarePlanetaryBox)
					]

					// Travels
					+ SHorizontalBox::Slot()
					.VAlign(VAlign_Top)
					.HAlign(HAlign_Right)
					.Padding(Theme.ContentPadding)
					[
						SNew(STextBlock)
						.TextStyle(&Theme.TextFont)
						.Text(this, &SFlareOrbitalMenu::GetTravelText)
						.WrapTextAt(0.8 * Theme.ContentWidth)
					]
				]

				// Bottom line
				+ SVerticalBox::Slot()
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				[
					SNew(SHorizontalBox)

					// Asta
					+ SHorizontalBox::Slot()
					[
						SAssignNew(AstaBox, SFlarePlanetaryBox)
					]

					// Hela
					+ SHorizontalBox::Slot()
					[
						SAssignNew(HelaBox, SFlarePlanetaryBox)
					]

					// Adena
					+ SHorizontalBox::Slot()
					[
						SAssignNew(AdenaBox, SFlarePlanetaryBox)
					]
				]
			]
		]
	];
}


/*----------------------------------------------------
	Interaction
----------------------------------------------------*/

void SFlareOrbitalMenu::Setup()
{
	SetEnabled(false);
	SetVisibility(EVisibility::Collapsed);
}

void SFlareOrbitalMenu::Enter()
{
	FLOG("SFlareOrbitalMenu::Enter");

	Game->DeactivateSector(MenuManager->GetPC());

	SetEnabled(true);
	SetVisibility(EVisibility::Visible);

	StopFastForward();

	UpdateMap();

	Game->SaveGame(MenuManager->GetPC());
}

void SFlareOrbitalMenu::Exit()
{
	SetEnabled(false);
	SetVisibility(EVisibility::Collapsed);

	NemaBox->ClearChildren();
	AnkaBox->ClearChildren();
	AstaBox->ClearChildren();
	HelaBox->ClearChildren();
	AdenaBox->ClearChildren();

	StopFastForward();

	Game->SaveGame(MenuManager->GetPC());
}

void SFlareOrbitalMenu::StopFastForward()
{
	TimeSinceFastForward = 0;
	FastForwardActive = false;
	FastForward->SetActive(false);
	Game->SaveGame(MenuManager->GetPC());
}

void SFlareOrbitalMenu::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	SCompoundWidget::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);

	if (IsEnabled() && MenuManager.IsValid())
	{
		// Check sector state changes
		for (int32 SectorIndex = 0; SectorIndex < MenuManager->GetPC()->GetCompany()->GetKnownSectors().Num(); SectorIndex++)
		{
			UFlareSimulatedSector* Sector = MenuManager->GetPC()->GetCompany()->GetKnownSectors()[SectorIndex];

			EFlareSectorBattleState::Type BattleState = Sector->GetSectorBattleState(MenuManager->GetPC()->GetCompany());
			if (LastSectorBattleState.Contains(Sector))
			{
				EFlareSectorBattleState::Type LastBattleState = LastSectorBattleState[Sector];

				// TODO more detail state and only for some transition
				if (LastBattleState == BattleState)
				{
					continue;
				}

				// Notify
				MenuManager->GetPC()->Notify(LOCTEXT("BattleStateChange", "Battle update"),
					FText::Format(LOCTEXT("BattleStateChangeFormat", "The military status of {0} has changed !"), Sector->GetSectorName()),
					FName("battle-state-changed"),
					EFlareNotification::NT_Military,
					10.0f,
					EFlareMenu::MENU_Sector,
					Sector);

				LastSectorBattleState[Sector] = BattleState;
			}
			else
			{
				LastSectorBattleState.Add(Sector,BattleState);
			}
		}

		// Fast forward every FastForwardPeriod
		if (FastForwardActive)
		{
			TimeSinceFastForward += InDeltaTime;
			if (TimeSinceFastForward > FastForwardPeriod)
			{
				MenuManager->GetGame()->GetGameWorld()->FastForward();
				TimeSinceFastForward = 0;
			}
		}
	}
}

void SFlareOrbitalMenu::UpdateMap()
{
	UpdateMapForBody(NemaBox, &Game->GetSectorCatalog()->OrbitalBodies[0]);
	UpdateMapForBody(AnkaBox, &Game->GetSectorCatalog()->OrbitalBodies[1]);
	UpdateMapForBody(AstaBox, &Game->GetSectorCatalog()->OrbitalBodies[2]);
	UpdateMapForBody(HelaBox, &Game->GetSectorCatalog()->OrbitalBodies[3]);
	UpdateMapForBody(AdenaBox, &Game->GetSectorCatalog()->OrbitalBodies[4]);
}

struct FSortByAltitudeAndPhase
{
	inline bool operator()(UFlareSimulatedSector& SectorA, UFlareSimulatedSector& SectorB) const
	{
		if (SectorA.GetOrbitParameters()->Altitude == SectorB.GetOrbitParameters()->Altitude)
		{
			// Compare phase
			if(SectorA.GetOrbitParameters()->Phase == SectorB.GetOrbitParameters()->Phase)
			{
				FLOGV("WARNING: %s and %s are at the same phase", *SectorA.GetSectorName().ToString(), *SectorB.GetSectorName().ToString())
			}

			return SectorA.GetOrbitParameters()->Phase < SectorB.GetOrbitParameters()->Phase;
		}
		else
		{
			return (SectorA.GetOrbitParameters()->Altitude < SectorB.GetOrbitParameters()->Altitude);
		}
	}
};

void SFlareOrbitalMenu::UpdateMapForBody(TSharedPtr<SFlarePlanetaryBox> Map, const FFlareSectorCelestialBodyDescription* Body)
{
	// Setup the planetary map
	Map->SetPlanetImage(&Body->CelestialBodyPicture);
	Map->SetRadius(Body->CelestialBodyRadiusOnMap);
	Map->ClearChildren();

	// Add the name
	Map->AddSlot()
	[
		SNew(STextBlock)
		.TextStyle(&FFlareStyleSet::GetDefaultTheme().SubTitleFont)
		.Text(Body->CelestialBodyName)
	];

	// Get sectors
	TArray<UFlareSimulatedSector*> KnownSectors = MenuManager->GetPC()->GetCompany()->GetKnownSectors();
	KnownSectors = KnownSectors.FilterByPredicate(
		[&](UFlareSimulatedSector* Sector)
		{
			return Sector->GetOrbitParameters()->CelestialBodyIdentifier == Body->CelestialBodyIdentifier;
		});
	KnownSectors.Sort(FSortByAltitudeAndPhase());

	// Add the sectors
	for (int32 SectorIndex = 0; SectorIndex < KnownSectors.Num(); SectorIndex++)
	{
		UFlareSimulatedSector* Sector = KnownSectors[SectorIndex];
		TSharedPtr<int32> IndexPtr(new int32(MenuManager->GetPC()->GetCompany()->GetKnownSectors().Find(Sector)));

		Map->AddSlot()
		[
			SNew(SFlareSectorButton)
			.Sector(Sector)
			.PlayerCompany(MenuManager->GetPC()->GetCompany())
			.OnClicked(this, &SFlareOrbitalMenu::OnOpenSector, IndexPtr)
		];
	}
}


/*----------------------------------------------------
	Callbacks
----------------------------------------------------*/

FText SFlareOrbitalMenu::GetFlyCurrentShipText() const
{
	UFlareSimulatedSpacecraft* CurrentShip = MenuManager->GetPC()->GetLastFlownShip();
	FText Reason;

	if (!CurrentShip)
	{
		return LOCTEXT("FlyCurrentNone", "Previous ship is unavailable");
	}
	else if (CurrentShip->CanBeFlown(Reason))
	{
		return FText::Format(LOCTEXT("FlyCurrentFormat", "Fly previous ship ({0})"), FText::FromName(CurrentShip->GetImmatriculation()));
	}
	else
	{
		return Reason;
	}
}

bool SFlareOrbitalMenu::IsFlyCurrentShipDisabled() const
{
	if (IsEnabled())
	{
		UFlareSimulatedSpacecraft* CurrentShip = MenuManager->GetPC()->GetLastFlownShip();
		FText Unused;

		if (CurrentShip && CurrentShip->CanBeFlown(Unused))
		{
			return false;
		}
	}

	return true;
}

FText SFlareOrbitalMenu::GetFlySelectedShipText() const
{
	UFlareSimulatedSpacecraft* CurrentShip = NULL;
	UFlareFleet* SelectedFleet = MenuManager->GetPC()->GetSelectedFleet();
	if (SelectedFleet && SelectedFleet->GetShips().Num() > 0)
	{
		CurrentShip = SelectedFleet->GetShips()[0];
	}

	FText Reason;
	if (!CurrentShip)
	{
		return LOCTEXT("FlySelectedNone", "No fleet selected");
	}
	else if (CurrentShip->CanBeFlown(Reason))
	{
		return FText::Format(LOCTEXT("FlySelectedFormat", "Fly selected ship ({0})"), FText::FromName(CurrentShip->GetImmatriculation()));
	}
	else
	{
		return Reason;
	}
}

bool SFlareOrbitalMenu::IsFlySelectedShipDisabled() const
{
	if (IsEnabled())
	{
		UFlareFleet* SelectedFleet = MenuManager->GetPC()->GetSelectedFleet();

		if (SelectedFleet && SelectedFleet->GetShips().Num() > 0)
		{
			FText Unused;
			UFlareSimulatedSpacecraft* CurrentShip = SelectedFleet->GetShips()[0];
			if (CurrentShip && CurrentShip->CanBeFlown(Unused))
			{
				return false;
			}
		}
	}

	return true;
}

FText SFlareOrbitalMenu::GetFastForwardText() const
{
	if (!IsEnabled())
	{
		return FText();
	}

	if (!FastForward->IsActive())
	{
		bool BattleInProgress = false;
		bool BattleLostWithRetreat = false;
		bool BattleLostWithoutRetreat = false;

		for (int32 SectorIndex = 0; SectorIndex < MenuManager->GetPC()->GetCompany()->GetKnownSectors().Num(); SectorIndex++)
		{
			UFlareSimulatedSector* Sector = MenuManager->GetPC()->GetCompany()->GetKnownSectors()[SectorIndex];

			EFlareSectorBattleState::Type BattleState = Sector->GetSectorBattleState(MenuManager->GetPC()->GetCompany());
			if (BattleState == EFlareSectorBattleState::Battle || BattleState == EFlareSectorBattleState::BattleNoRetreat)
			{
				BattleInProgress = true;
			}
			else if (BattleState == EFlareSectorBattleState::BattleLost)
			{
				BattleLostWithRetreat = true;
			}
			else if (BattleState == EFlareSectorBattleState::BattleLostNoRetreat)
			{
				BattleLostWithoutRetreat = true;
			}
		}

		if (BattleInProgress)
		{
			return LOCTEXT("NoFastForwardBattleText", "Battle in progress");
		}
		else if (BattleLostWithRetreat)
		{
			return LOCTEXT("FastForwardBattleLostWithRetreatText", "Fast forward (!)");
		}
		else if (BattleLostWithoutRetreat)
		{
			return LOCTEXT("FastForwardBattleLostWithoutRetreatText", "Fast forward (!)");
		}
		else
		{
			return LOCTEXT("FastForwardText", "Fast forward");
		}
	}
	else
	{
		return LOCTEXT("FastForwardingText", "Fast forwarding...");
	}
}

const FSlateBrush* SFlareOrbitalMenu::GetFastForwardIcon() const
{
	if (FastForward->IsActive())
	{
		return FFlareStyleSet::GetIcon("Stop");
	}
	else
	{
		return FFlareStyleSet::GetIcon("FastForward");
	}
}

bool SFlareOrbitalMenu::IsFastForwardDisabled() const
{
	if (IsEnabled())
	{
		UFlareWorld* GameWorld = MenuManager->GetGame()->GetGameWorld();

		for (int32 SectorIndex = 0; SectorIndex < MenuManager->GetPC()->GetCompany()->GetKnownSectors().Num(); SectorIndex++)
		{
			UFlareSimulatedSector* Sector = MenuManager->GetPC()->GetCompany()->GetKnownSectors()[SectorIndex];

			EFlareSectorBattleState::Type BattleState = Sector->GetSectorBattleState(MenuManager->GetPC()->GetCompany());
			if(BattleState == EFlareSectorBattleState::Battle || BattleState == EFlareSectorBattleState::BattleNoRetreat)
			{
				return true;
			}
		}
		
		if (GameWorld && (GameWorld->GetTravels().Num() > 0 || true)) // Not true if there is pending todo event
		{
			// TODO ALPHA : show the button during station/ship constructions as well
			return false;
		}
	}

	return true;
}

FText SFlareOrbitalMenu::GetDateText() const
{
	if (IsEnabled())
	{
		UFlareWorld* GameWorld = MenuManager->GetGame()->GetGameWorld();
		UFlareCompany* PlayerCompany = MenuManager->GetPC()->GetCompany();

		if (GameWorld && PlayerCompany)
		{
			uint64 Credits = PlayerCompany->GetMoney();
			FText DateText = UFlareGameTools::GetDisplayDate(GameWorld->GetDate());
			return FText::Format(LOCTEXT("DateCreditsInfoFormat", "\n{0} - {1} credits"), DateText, FText::AsNumber(UFlareGameTools::DisplayMoney(Credits)));
		}
	}

	return FText();
}

FText SFlareOrbitalMenu::GetTravelText() const
{
	if (IsEnabled())
	{
		UFlareWorld* GameWorld = MenuManager->GetGame()->GetGameWorld();
		if (GameWorld)
		{
			FString Result = "\n";

			// List travels
			for (int32 TravelIndex = 0; TravelIndex < GameWorld->GetTravels().Num(); TravelIndex++)
			{
				UFlareTravel* Travel = GameWorld->GetTravels()[TravelIndex];
				if (Travel->GetFleet()->GetFleetCompany() == MenuManager->GetPC()->GetCompany())
				{
					FText TravelText = FText::Format(LOCTEXT("TravelTextFormat", "{0} is travelling to {1} ({2} left)"),
						Travel->GetFleet()->GetFleetName(),
						Travel->GetDestinationSector()->GetSectorName(),
						FText::FromString(*UFlareGameTools::FormatDate(Travel->GetRemainingTravelDuration(), 1))); //FString needed here

					Result += TravelText.ToString() + "\n";
				}
			}

			// List ship productions
			TArray<UFlareCompany*> Companies = GameWorld->GetCompanies();
			for (int32 CompanyIndex = 0; CompanyIndex < Companies.Num(); CompanyIndex++)
			{
				TArray<UFlareSimulatedSpacecraft*>& CompanyStations = Companies[CompanyIndex]->GetCompanyStations();
				for (int32 StationIndex = 0; StationIndex < CompanyStations.Num(); StationIndex++)
				{
					for (int32 FactoryIndex = 0; FactoryIndex < CompanyStations[StationIndex]->GetFactories().Num(); FactoryIndex++)
					{
						// Get shipyard if existing
						UFlareFactory* TargetFactory = CompanyStations[StationIndex]->GetFactories()[FactoryIndex];
						if (!TargetFactory->IsShipyard())
						{
							continue;
						}

						// TODO FRED : API for the list of current orders ?

						// Look for one of our ships
						if (TargetFactory->GetOrderShipCompany() == MenuManager->GetPC()->GetCompany()->GetIdentifier())
						{
							FText TravelText = FText::Format(LOCTEXT("ShipProductionTextFormat", "A {0} is being built by {1} ({2} left)"),
								MenuManager->GetGame()->GetSpacecraftCatalog()->Get(TargetFactory->GetOrderShipClass())->Name,
								Companies[CompanyIndex]->GetCompanyName(),
								FText::FromString(*UFlareGameTools::FormatDate(42424242, 1))); //FString needed here
						}
					}
				}
			}

			return FText::FromString(Result);
		}
	}

	return FText();
}

FVector2D SFlareOrbitalMenu::GetWidgetPosition(int32 Index) const
{
	return FVector2D(1920, 1080) / 2;
}

FVector2D SFlareOrbitalMenu::GetWidgetSize(int32 Index) const
{
	int WidgetSize = 200;
	FVector2D BaseSize(WidgetSize, WidgetSize);
	return BaseSize;
}


/*----------------------------------------------------
	Action callbacks
----------------------------------------------------*/

void SFlareOrbitalMenu::OnInspectCompany()
{
	MenuManager->OpenMenu(EFlareMenu::MENU_Company);
}

void SFlareOrbitalMenu::OnInspectFleet()
{
	MenuManager->OpenMenu(EFlareMenu::MENU_Fleet);
}

void SFlareOrbitalMenu::OnOpenLeaderboard()
{
	MenuManager->OpenMenu(EFlareMenu::MENU_Leaderboard);
}

void SFlareOrbitalMenu::OnMainMenu()
{
	AFlarePlayerController* PC = MenuManager->GetPC();

	PC->CompleteObjective();
	MenuManager->FlushNotifications();
	MenuManager->OpenMenu(EFlareMenu::MENU_Main);

	Game->SaveGame(PC);
}

void SFlareOrbitalMenu::OnOpenSector(TSharedPtr<int32> Index)
{
	UFlareSimulatedSector* Sector = MenuManager->GetPC()->GetCompany()->GetKnownSectors()[*Index];
	MenuManager->OpenMenu(EFlareMenu::MENU_Sector, Sector);
}

void SFlareOrbitalMenu::OnFastForwardClicked()
{
	if (FastForward->IsActive())
	{
		bool BattleInProgress = false;
		bool BattleLostWithRetreat = false;
		bool BattleLostWithoutRetreat = false;

		for (int32 SectorIndex = 0; SectorIndex < MenuManager->GetPC()->GetCompany()->GetKnownSectors().Num(); SectorIndex++)
		{
			UFlareSimulatedSector* Sector = MenuManager->GetPC()->GetCompany()->GetKnownSectors()[SectorIndex];

			EFlareSectorBattleState::Type BattleState = Sector->GetSectorBattleState(MenuManager->GetPC()->GetCompany());
			if(BattleState == EFlareSectorBattleState::Battle || BattleState == EFlareSectorBattleState::BattleNoRetreat)
			{
				BattleInProgress = true;
			}
			else if(BattleState == EFlareSectorBattleState::BattleLost)
			{
				BattleLostWithRetreat = true;
			}
			else if(BattleState == EFlareSectorBattleState::BattleLostNoRetreat)
			{
				BattleLostWithoutRetreat = true;
			}
		}

		if (BattleInProgress)
		{
			return;
		}
		else if (BattleLostWithoutRetreat)
		{
			MenuManager->Confirm(LOCTEXT("ConfirmBattleLostWithoutRetreatTitle", "SACRIFICE SHIPS ?"),
								 LOCTEXT("ConfirmBattleLostWithoutRetreatText", "Some of the ships engaged in a battle cannot retreat and will be lost."),
								 FSimpleDelegate::CreateSP(this, &SFlareOrbitalMenu::OnFastForwardConfirmed));

		}
		else if (BattleLostWithRetreat)
		{
			MenuManager->Confirm(LOCTEXT("ConfirmBattleLostWithRetreatTitle", "SACRIFICE SHIPS ?"),
								 LOCTEXT("ConfirmBattleLostWithRetreatText", "Some of the ships engaged in a battle can still retreat ! They will be lost."),
								 FSimpleDelegate::CreateSP(this, &SFlareOrbitalMenu::OnFastForwardConfirmed));
		}
		else
		{
			OnFastForwardConfirmed();
		}
	}
	else
	{
		FastForwardActive = false;
	}
}


void SFlareOrbitalMenu::OnFastForwardConfirmed()
{
	FastForwardActive = true;
	Game->SaveGame(MenuManager->GetPC());
}

void SFlareOrbitalMenu::OnFlyCurrentShipClicked()
{
	AFlarePlayerController* PC = MenuManager->GetPC();
	UFlareSimulatedSpacecraft* CurrentShip = PC->GetLastFlownShip();

	if (CurrentShip)
	{
		UFlareSimulatedSector* Sector = CurrentShip->GetCurrentSector();
		Sector->SetShipToFly(CurrentShip);
		MenuManager->OpenMenu(EFlareMenu::MENU_ActivateSector, Sector);
	}
}

void SFlareOrbitalMenu::OnFlySelectedShipClicked()
{
	AFlarePlayerController* PC = MenuManager->GetPC();
	UFlareFleet* SelectedFleet = MenuManager->GetPC()->GetSelectedFleet();
	if (SelectedFleet->GetShips().Num() == 0)
	{
		return;
	}

	UFlareSimulatedSpacecraft* CurrentShip = SelectedFleet->GetShips()[0];

	if (CurrentShip)
	{
		UFlareSimulatedSector* Sector = CurrentShip->GetCurrentSector();
		Sector->SetShipToFly(CurrentShip);
		MenuManager->OpenMenu(EFlareMenu::MENU_ActivateSector, Sector);
	}
}

#undef LOCTEXT_NAMESPACE

