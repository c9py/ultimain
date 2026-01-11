# Dream-Vortex

**Unified AI Storytelling & Business Simulation Platform**

Dream-Vortex integrates two powerful systems into a cohesive platform:

- **DreamCog** - Advanced AI storytelling with sophisticated character modeling, world-building, and narrative generation
- **Vorticog** - Business simulation engine with AI-powered agent behavior and economic systems

## Overview

Vorticog combines strategic business management with rich narrative experiences through semi-autonomous agents, dynamic relationships, and world-building features. Players manage companies, make strategic decisions, and interact with AI-driven characters in a living, evolving world.

## Core Features

### Business Simulation
- **Company Management**: Build and grow your business empire
- **Business Units**: Manage offices, stores, factories, mines, farms, and laboratories
- **Resource Production**: Extract, process, and manufacture goods
- **Market Trading**: Buy and sell goods in dynamic markets
- **Employee Management**: Hire, train, and manage workers
- **Financial Systems**: Handle cash flow, loans, investments, and taxes
- **Technology Research**: Unlock new capabilities through R&D
- **Competition**: Compete with other players and AI companies

### Advanced Agentic Simulation
Powered by sophisticated AI models for realistic character behavior:

#### Character System
- **Personality Models**: Big Five personality traits (Openness, Conscientiousness, Extraversion, Agreeableness, Neuroticism)
- **Communication Styles**: Customizable formality, verbosity, and emotional expression
- **Behavioral Tendencies**: Impulsiveness, risk-taking, empathy, leadership, independence
- **Character Personas**: Pre-defined personality templates (Ambitious Leader, Cautious Analyst, etc.)
- **Character Traits**: Professional, social, cognitive, and emotional characteristics

#### Agent Types
- **Customers**: End users with preferences and buying patterns
- **Suppliers**: Providers with reliability and negotiation styles
- **Employees**: Workers with skills, emotions, and career goals
- **Partners**: Business allies with varying commitment levels
- **Investors**: Financial backers with risk tolerance
- **Competitors**: Rival entities with strategic behaviors

#### Emotional Intelligence
- **Emotional States**: Happiness, satisfaction, stress, loyalty, trust
- **Psychological Needs**: Financial security, recognition, autonomy, social connection
- **Motivation System**: Short-term goals, long-term aspirations, core values
- **Memory System**: Experience tracking, knowledge accumulation, trauma, achievements

#### Relationship Dynamics
- **Relationship Types**: Business, personal, professional, familial, competitive
- **Dynamic Metrics**: Trust, affection, respect, loyalty, dependency, tension
- **Relationship Events**: Track significant moments and their impacts
- **Social Networks**: Complex multi-agent relationship graphs

#### Group Structures
- **Group Types**: Departments, teams, unions, associations, clubs, networks
- **Group Dynamics**: Cohesion, influence, morale
- **Membership Roles**: Leaders, core members, regular members, associates
- **Communities**: City-level social structures

#### Event System
- **Event Types**: Interactions, transactions, milestones, crises, celebrations, conflicts, negotiations
- **Time-Sensitive Processing**: Scheduled events with automatic execution
- **Impact Tracking**: Emotional and relationship changes
- **Event Triggers**: Automated event generation based on conditions

### World Building & Storytelling (DreamCog Integration)

Enhanced narrative capabilities for rich storytelling:

#### World System
- **World Creation**: Define genre, time period, technology level, magic systems
- **World Rules**: Custom physics, social norms, and magical systems
- **Cultural Context**: Rich cultural and historical backgrounds

#### Location Hierarchy
- **Location Types**: Cities, buildings, wilderness, dungeons, realms, dimensions
- **Nested Structures**: Parent-child location relationships
- **Location Attributes**: Climate, population, danger levels, resources

#### Lore Database
- **Lore Categories**: History, legends, culture, religion, politics, science, magic, species
- **Knowledge Management**: Public and secret information
- **Tagging System**: Organize and search lore entries

#### Timeline Management
- **World Events**: Historical timeline with major events
- **Event Importance**: 1-10 significance rating
- **Flexible Dating**: Support for fictional calendars
- **Consequences Tracking**: Long-term effects of events

#### Scheduled Events
- **Time-Based Triggers**: Events at specific times
- **Condition-Based Triggers**: Events based on game state
- **Recurring Events**: Automatic periodic occurrences
- **Priority System**: Control execution order

## Architecture

### Tech Stack
- **Frontend**: React 19, TypeScript, Vite, Tailwind CSS
- **Backend**: Express, tRPC, TypeScript
- **Database**: MySQL with Drizzle ORM
- **Authentication**: JWT-based with OAuth support
- **State Management**: TanStack Query (React Query)

### Project Structure
```
├── client/          # React frontend
│   ├── components/  # UI components
│   └── pages/       # Page components
├── server/          # Express backend
│   ├── _core/       # Core server infrastructure
│   ├── db.ts        # Database functions
│   └── routers.ts   # API routes
├── shared/          # Shared types and utilities
├── drizzle/         # Database schema and migrations
│   └── schema.ts    # Table definitions
└── docs/            # Documentation
    ├── AGENTIC_SIMULATION.md      # Agentic system docs
    └── DREAMCOG_INTEGRATION.md    # World building docs
```

## Getting Started

### Prerequisites
- Node.js 18+
- pnpm package manager
- MySQL database

### Installation

1. Clone the repository:
```bash
git clone https://github.com/o9nn/dream-vortex.git
cd dream-vortex
```

2. Install dependencies:
```bash
pnpm install
```

3. Configure environment:
```bash
cp .env.example .env
# Edit .env with your database and API credentials
```

4. Set up database:
```bash
npm run db:push
```

5. Start development server:
```bash
npm run dev
```

The application will be available at `http://localhost:5000`.

## Development

### Available Scripts

```bash
npm run dev      # Start development server
npm run build    # Build for production
npm run start    # Start production server
npm run check    # TypeScript type checking
npm run format   # Format code with Prettier
npm run test     # Run tests
npm run db:push  # Apply database migrations
```

### Database Schema

The system uses a comprehensive schema with 27+ tables:

**Core Tables**:
- `users`, `companies`, `business_units`, `cities`
- `resource_types`, `inventory`, `production_recipes`
- `market_listings`, `transactions`, `notifications`

**Agentic Simulation Tables**:
- `character_personas`, `character_traits`
- `agents`, `agent_traits`, `agent_histories`
- `relationships`, `relationship_events`
- `agent_groups`, `group_memberships`
- `communities`, `community_memberships`
- `agent_events`, `event_triggers`

**DreamCog Integration Tables**:
- `agent_big_five_personality`
- `agent_motivations`, `agent_memories`
- `worlds`, `locations`, `lore_entries`
- `world_events`, `scheduled_world_events`

See [docs/AGENTIC_SIMULATION.md](./docs/AGENTIC_SIMULATION.md) and [docs/DREAMCOG_INTEGRATION.md](./docs/DREAMCOG_INTEGRATION.md) for detailed documentation.

## API Overview

The API is built with tRPC for end-to-end type safety. Key routers include:

### Business Management
- `company.*` - Company operations
- `unit.*` - Business unit management
- `production.*` - Production and manufacturing
- `market.*` - Market trading
- `transaction.*` - Financial transactions

### Agentic Simulation
- `persona.*` - Character personas
- `trait.*` - Character traits
- `agent.*` - Agent operations
- `relationship.*` - Relationship management
- `group.*` - Group operations
- `community.*` - Community management
- `event.*` - Event processing

### DreamCog Integration
- `personality.*` - Big Five personality
- `motivation.*` - Goal tracking
- `memory.*` - Experience tracking
- `relationshipEvent.*` - Relationship history
- `world.*` - World building
- `location.*` - Location hierarchy
- `lore.*` - Lore management
- `worldEvent.*` - Timeline events
- `scheduledWorldEvent.*` - Scheduled events

## Key Concepts

### Semi-Autonomous Agents
Agents make decisions based on:
- Personality traits and behavioral tendencies
- Current emotional state and psychological needs
- Memories and past experiences
- Relationships with other agents
- Motivations and goals
- Group and community influences

### Event-Driven Simulation
- Events shape narrative and game state
- Emotional impacts affect agent behavior
- Relationship impacts change social dynamics
- Historical tracking enables rich narratives
- Scheduled events create anticipation

### Dynamic Relationships
- Relationships evolve through interactions
- Significant moments are recorded as events
- Trust, affection, and respect change over time
- Social networks influence agent behavior
- Group dynamics affect individual choices

## Documentation

- **[AGENTIC_SIMULATION.md](./docs/AGENTIC_SIMULATION.md)**: Core agentic simulation system
- **[DREAMCOG_INTEGRATION.md](./docs/DREAMCOG_INTEGRATION.md)**: Advanced AI storytelling features
- **[IMPLEMENTATION_SUMMARY.md](./docs/IMPLEMENTATION_SUMMARY.md)**: Implementation details

## Testing

Run the test suite:
```bash
npm run test
```

The test suite covers:
- Database operations
- API endpoints
- Type validation
- Authorization checks
- Business logic

## Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes with tests
4. Run linting and tests
5. Submit a pull request

## Performance

- Full TypeScript type safety across the stack
- Optimized database queries with proper indexing
- Efficient relationship traversal
- Scalable schema design for large datasets
- Type-safe API with automatic validation

## Security

- JWT-based authentication
- SQL injection prevention via Drizzle ORM
- Input validation with Zod schemas
- Authorization checks on protected operations
- Secure credential storage

## Related Projects

- **[DreamCog](https://github.com/o9nn/dreamcog)**: Advanced AI storytelling platform
- **[Vorticog](https://github.com/o9nn/vorticog)**: Business simulation engine
- **[DreamGen](https://github.com/o9nn/dreamgen)**: Base DreamGen API integration

## License

MIT

## Acknowledgments

- DreamGen for AI inspiration
- The open-source community for excellent tools
- All contributors to the project

## Support

For issues and questions:
- GitHub Issues: [https://github.com/o9nn/dream-vortex/issues](https://github.com/o9nn/dream-vortex/issues)
- Documentation: See [/docs](./docs) directory

---

Built with ❤️ for the business simulation and storytelling community
