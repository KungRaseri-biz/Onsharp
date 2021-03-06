﻿namespace Onsharp.Entities.Factory
{
    /// <summary>
    /// The factory which manages the creation of <see cref="Vehicle"/> objects.
    /// </summary>
    internal class VehicleFactory : IEntityFactory<Vehicle>
    {
        public Vehicle Create(int id)
        {
            return new Vehicle(id);
        }
    }
}