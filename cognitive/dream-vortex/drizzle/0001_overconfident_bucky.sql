CREATE TABLE `business_units` (
	`id` int AUTO_INCREMENT NOT NULL,
	`companyId` int NOT NULL,
	`type` enum('office','store','factory','mine','farm','laboratory') NOT NULL,
	`name` varchar(128) NOT NULL,
	`cityId` int NOT NULL,
	`level` int NOT NULL DEFAULT 1,
	`size` int NOT NULL DEFAULT 100,
	`condition` int NOT NULL DEFAULT 100,
	`efficiency` decimal(5,2) NOT NULL DEFAULT '1.00',
	`isActive` boolean NOT NULL DEFAULT true,
	`createdAt` timestamp NOT NULL DEFAULT (now()),
	`updatedAt` timestamp NOT NULL DEFAULT (now()) ON UPDATE CURRENT_TIMESTAMP,
	CONSTRAINT `business_units_id` PRIMARY KEY(`id`)
);
--> statement-breakpoint
CREATE TABLE `cities` (
	`id` int AUTO_INCREMENT NOT NULL,
	`name` varchar(128) NOT NULL,
	`country` varchar(64) NOT NULL,
	`population` int NOT NULL,
	`wealthIndex` decimal(5,2) NOT NULL DEFAULT '1.00',
	`taxRate` decimal(5,4) NOT NULL DEFAULT '0.2000',
	`latitude` decimal(10,7),
	`longitude` decimal(10,7),
	CONSTRAINT `cities_id` PRIMARY KEY(`id`)
);
--> statement-breakpoint
CREATE TABLE `companies` (
	`id` int AUTO_INCREMENT NOT NULL,
	`userId` int NOT NULL,
	`name` varchar(128) NOT NULL,
	`logo` varchar(512),
	`description` text,
	`cash` decimal(20,2) NOT NULL DEFAULT '1000000.00',
	`reputation` int NOT NULL DEFAULT 50,
	`founded` timestamp NOT NULL DEFAULT (now()),
	`createdAt` timestamp NOT NULL DEFAULT (now()),
	`updatedAt` timestamp NOT NULL DEFAULT (now()) ON UPDATE CURRENT_TIMESTAMP,
	CONSTRAINT `companies_id` PRIMARY KEY(`id`)
);
--> statement-breakpoint
CREATE TABLE `company_technologies` (
	`id` int AUTO_INCREMENT NOT NULL,
	`companyId` int NOT NULL,
	`technologyId` int NOT NULL,
	`researchProgress` int NOT NULL DEFAULT 0,
	`isCompleted` boolean NOT NULL DEFAULT false,
	`completedAt` timestamp,
	CONSTRAINT `company_technologies_id` PRIMARY KEY(`id`)
);
--> statement-breakpoint
CREATE TABLE `employees` (
	`id` int AUTO_INCREMENT NOT NULL,
	`businessUnitId` int NOT NULL,
	`count` int NOT NULL DEFAULT 0,
	`qualification` decimal(5,2) NOT NULL DEFAULT '1.00',
	`salary` decimal(12,2) NOT NULL DEFAULT '1000.00',
	`morale` int NOT NULL DEFAULT 70,
	`updatedAt` timestamp NOT NULL DEFAULT (now()) ON UPDATE CURRENT_TIMESTAMP,
	CONSTRAINT `employees_id` PRIMARY KEY(`id`)
);
--> statement-breakpoint
CREATE TABLE `game_state` (
	`id` int AUTO_INCREMENT NOT NULL,
	`currentTurn` int NOT NULL DEFAULT 1,
	`turnDuration` int NOT NULL DEFAULT 3600,
	`lastTurnProcessed` timestamp,
	`settings` json,
	CONSTRAINT `game_state_id` PRIMARY KEY(`id`)
);
--> statement-breakpoint
CREATE TABLE `inventory` (
	`id` int AUTO_INCREMENT NOT NULL,
	`businessUnitId` int NOT NULL,
	`resourceTypeId` int NOT NULL,
	`quantity` decimal(20,4) NOT NULL DEFAULT '0',
	`quality` decimal(5,2) NOT NULL DEFAULT '1.00',
	`averageCost` decimal(12,2) NOT NULL DEFAULT '0',
	`updatedAt` timestamp NOT NULL DEFAULT (now()) ON UPDATE CURRENT_TIMESTAMP,
	CONSTRAINT `inventory_id` PRIMARY KEY(`id`)
);
--> statement-breakpoint
CREATE TABLE `market_listings` (
	`id` int AUTO_INCREMENT NOT NULL,
	`companyId` int NOT NULL,
	`businessUnitId` int NOT NULL,
	`resourceTypeId` int NOT NULL,
	`quantity` decimal(20,4) NOT NULL,
	`quality` decimal(5,2) NOT NULL DEFAULT '1.00',
	`pricePerUnit` decimal(12,2) NOT NULL,
	`cityId` int NOT NULL,
	`isActive` boolean NOT NULL DEFAULT true,
	`createdAt` timestamp NOT NULL DEFAULT (now()),
	`expiresAt` timestamp,
	CONSTRAINT `market_listings_id` PRIMARY KEY(`id`)
);
--> statement-breakpoint
CREATE TABLE `notifications` (
	`id` int AUTO_INCREMENT NOT NULL,
	`userId` int NOT NULL,
	`type` enum('info','success','warning','error','trade','production') NOT NULL,
	`title` varchar(256) NOT NULL,
	`message` text,
	`isRead` boolean NOT NULL DEFAULT false,
	`createdAt` timestamp NOT NULL DEFAULT (now()),
	CONSTRAINT `notifications_id` PRIMARY KEY(`id`)
);
--> statement-breakpoint
CREATE TABLE `production_queue` (
	`id` int AUTO_INCREMENT NOT NULL,
	`businessUnitId` int NOT NULL,
	`recipeId` int NOT NULL,
	`quantity` decimal(12,4) NOT NULL,
	`progress` decimal(5,2) NOT NULL DEFAULT '0',
	`isActive` boolean NOT NULL DEFAULT true,
	`createdAt` timestamp NOT NULL DEFAULT (now()),
	CONSTRAINT `production_queue_id` PRIMARY KEY(`id`)
);
--> statement-breakpoint
CREATE TABLE `production_recipes` (
	`id` int AUTO_INCREMENT NOT NULL,
	`unitType` enum('factory','farm','mine','laboratory') NOT NULL,
	`outputResourceId` int NOT NULL,
	`outputQuantity` decimal(12,4) NOT NULL DEFAULT '1',
	`inputResources` json,
	`laborRequired` int NOT NULL DEFAULT 1,
	`timeRequired` int NOT NULL DEFAULT 1,
	`technologyRequired` int DEFAULT 0,
	`description` text,
	CONSTRAINT `production_recipes_id` PRIMARY KEY(`id`)
);
--> statement-breakpoint
CREATE TABLE `resource_types` (
	`id` int AUTO_INCREMENT NOT NULL,
	`code` varchar(64) NOT NULL,
	`name` varchar(128) NOT NULL,
	`category` enum('raw_material','intermediate','finished_good','equipment','consumable') NOT NULL,
	`basePrice` decimal(12,2) NOT NULL,
	`unit` varchar(32) NOT NULL DEFAULT 'units',
	`icon` varchar(128),
	`description` text,
	CONSTRAINT `resource_types_id` PRIMARY KEY(`id`),
	CONSTRAINT `resource_types_code_unique` UNIQUE(`code`)
);
--> statement-breakpoint
CREATE TABLE `technologies` (
	`id` int AUTO_INCREMENT NOT NULL,
	`code` varchar(64) NOT NULL,
	`name` varchar(128) NOT NULL,
	`category` enum('production','commerce','management','science') NOT NULL,
	`description` text,
	`researchCost` int NOT NULL,
	`prerequisites` json,
	`effects` json,
	CONSTRAINT `technologies_id` PRIMARY KEY(`id`),
	CONSTRAINT `technologies_code_unique` UNIQUE(`code`)
);
--> statement-breakpoint
CREATE TABLE `transactions` (
	`id` int AUTO_INCREMENT NOT NULL,
	`type` enum('purchase','sale','salary','tax','construction','maintenance','other') NOT NULL,
	`companyId` int NOT NULL,
	`amount` decimal(20,2) NOT NULL,
	`description` text,
	`relatedUnitId` int,
	`relatedResourceId` int,
	`createdAt` timestamp NOT NULL DEFAULT (now()),
	CONSTRAINT `transactions_id` PRIMARY KEY(`id`)
);
